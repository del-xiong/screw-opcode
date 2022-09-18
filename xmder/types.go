package main

import "time"

var sourcepath string

type cryptStats struct {
	TotalFiles int64 // 总数
	SuccessCryptedCount int64 // 成功加密数
	FailedCryptedCount int64 // 失败数
	TimeStart time.Time
	TimeEnd time.Time
}