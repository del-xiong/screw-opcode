# screw_opcode
基于opcache魔改进行php加密，在生成和读取opcode阶段进行加解密从而有效防止源码泄露。

已支持php版本  
php 7.2 主分支 (https://github.com/del-xiong/screw-opcode)  
php8.0 php-8.0分支

# 安装

编译流程
1. 下载本程序并解压到某个目录并进入
2. 执行 **bash gensecretkey.sh** 生成随机安全密钥头文件
3. 执行php bin中的**phpize**自动生成扩展所需文件（如果你的php里没有可以去官网下载）
4. 执行 **./configure --with-php-config=[php config path]** 进行配置，[php config path]是你的php-config的绝对路径
5. 执行**make**生成扩展 modules/screwopcode.so
6. **make install**安装扩展或手动把扩展路径加入php.ini中 重启php
```
注意由于是zend插件，需要用zend_extension来载入
zend_extension=/path/to/screwopcode.so
```
7. 新建opcode缓存目录(假设为/tmp/opcodes)并修改php.ini配置文件，添加以下配置
```
[screwopcode]
screwopcode.enable = 1
screwopcode.file_cache=/tmp/opcodes
screwopcode.file_cache_only=0
screwopcode.file_override_enabled=0
screwopcode.enable_cli=1
screwopcode.jit=0
screwopcode.validate_timestamps=0
```
8. 执行php -m 确认[Zend Modules]中包含ScrewOpcode说开插件加载成功

**/tmp/opcodes 是opcode加密缓存目录 可以自定义到其他目录，必须先创建目录否则可能出错**

# 如何使用
插件安装成功后，运行php的时候会自动生成opcode缓存并移除源代码，下次执行相同文件直接从opcode加载。
你也可以在项目发布前使用小工具手动批量加密，进入xmder目录并执行 ./xmder -p [你的源码目录] 即可(目前还未添加并发处理所以速度可能会比较慢)

# 项目持续升级中 企鹅群 530887505
