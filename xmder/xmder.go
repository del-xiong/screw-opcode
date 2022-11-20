package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	"strings"
	"sync/atomic"
	"syscall"
	"time"
)
var cmd *exec.Cmd

const cryptedHeaderContent = "<?php /**screwopcode**/exit;?>"

func init() {

	flag.StringVar(&sourcepath,"p","","source code path")
	flag.Parse()
	if sourcepath == "" {
		log.Fatalln("source path not defined")
	}
}

func main() {
	stats := cryptStats{
		TimeStart: time.Now(),
	}
	loopVisitEachFile(sourcepath, &stats)
	stats.TimeEnd = time.Now()
	stats.TotalFiles = stats.SuccessCryptedCount+stats.FailedCryptedCount
	log.Printf("total files(总文件数): %d\n", stats.TotalFiles)
	log.Printf("success crypted count(成功加密文件数): %d\n", stats.SuccessCryptedCount)
	log.Printf("failed crypted count(失败文件数): %d\n", stats.FailedCryptedCount)
	log.Printf("processing time(耗时): %.2fS\n", stats.TimeEnd.Sub(stats.TimeStart).Seconds())
}

func loopVisitEachFile(dirpath string, stats *cryptStats) {
	filepath.Walk(dirpath, func(p string, f os.FileInfo, err error) error {
		if f == nil {
			return err
		}
		if !f.IsDir() && len(p) > 4 && strings.ToLower(p[len(p)-4:]) == ".php" {
			silentExecuteSourceFile(p, f, stats)
		} else if f.IsDir() {
			loopVisitEachFile(path.Join(dirpath, f.Name()), stats)
			return nil
		}
		return nil
	})
}

func silentExecuteSourceFile(fPath string,fInfo os.FileInfo,stats *cryptStats) {
	// check if this file has been crypted or not
	// 先检查文件是否已加密
	if fInfo.Size() == int64(len(cryptedHeaderContent)) {
		b, err := os.ReadFile(fPath)
		if err ==nil && string(b) == cryptedHeaderContent {
			// crypted return
			fmt.Print(fPath+"...skip..√\n")
			atomic.AddInt64(&stats.SuccessCryptedCount, 1)
			return
		}
	}
	fmt.Print(fPath+"...")
	cmd = exec.Command(os.Getenv("SHELL"),"-c",fmt.Sprintf(`/usr/bin/php -r "screwopcode_compile_file('%s');"`, fPath))
	cmd.SysProcAttr = &syscall.SysProcAttr{}
	uid, gid := GetFileGroupId(fInfo)
	cmd.SysProcAttr.Credential = &syscall.Credential{Uid: uid, Gid: gid}
	b, err := cmd.CombinedOutput()
	fmt.Print("√\n")
	b2, err := os.ReadFile(fPath)
	if err != nil {
		atomic.AddInt64(&stats.FailedCryptedCount, 1)
		log.Fatalln(err)
		return
	}
	if string(b2) != cryptedHeaderContent {
		atomic.AddInt64(&stats.FailedCryptedCount, 1)
		log.Println(string(b))
	} else {
		atomic.AddInt64(&stats.SuccessCryptedCount, 1)
	}
}
// get uid/gid of a file
func GetFileGroupId(fInfo os.FileInfo) (uint32,uint32) {
	stat,_ := fInfo.Sys().(*syscall.Stat_t)
	return uint32(stat.Uid),uint32(stat.Gid)
}
