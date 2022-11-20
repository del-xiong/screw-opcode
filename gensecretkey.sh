#!/bin/bash
pwd_sha256=`head -20 /dev/urandom |sha256sum`
pwd_sha256=${pwd_sha256:0:64}
if [ ! `echo $pwd_sha256|wc -L` == 64  ];then
  echo "wrong data";
  echo "数据错误";
  exit
fi
echo "rand secret key(随机秘钥): $pwd_sha256";
rm -rf screwopcode_key.h;
echo "#define GK_F(x,y,z) ((x & y) | (~x & z))
#define GK_G(x,y,z) ((x & z) | (y & ~z))
#define GK_H(x,y,z) (x^y^z)
#define GK_I(x,y,z) (y ^ (x | ~z))
#define GK_ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
#define GK_TRANSFORM1(a,b,c,d,x,s,ac) \
          { \
          a += GK_F(b,c,d) + x + ac; \
          a = GK_ROTATE_LEFT(a,s); \
          a += b; \
          }
#define GK_TRANSFORM2(a,b,c,d,x,s,ac) \
          { \
          a += GK_G(b,c,d) + x + ac; \
          a = GK_ROTATE_LEFT(a,s); \
          a += b; \
          }
#define GK_TRANSFORM3(a,b,c,d,x,s,ac) \
          { \
          a += GK_H(b,c,d) + x + ac; \
          a = GK_ROTATE_LEFT(a,s); \
          a += b; \
          }
#define GK_TRANSFORM4(a,b,c,d,x,s,ac) \
          { \
          a += GK_I(b,c,d) + x + ac; \
          a = GK_ROTATE_LEFT(a,s); \
          a += b; \
          }
#define GETKEY() ({unsigned int state[16];\\" >> screwopcode_key.h;
for i in {0..15}
do
echo "state[$i] = 0x${pwd_sha256:$i*4:4};\\" >> screwopcode_key.h;
done
# 循环创建变形逻辑
for ((i=0;i<1+$((16#${pwd_sha256:0:1}));i++))
do
echo "GK_TRANSFORM1(state[0], state[1], state[2], state[3], state[0], 1, (state[0]^state[1]) ^ 0x${pwd_sha256:3:2});\\
GK_TRANSFORM2(state[1], state[2], state[3], state[4], state[1], 27, (state[1]^state[2]) ^ 0x${pwd_sha256:5:2});\\
GK_TRANSFORM3(state[2], state[3], state[4], state[5], state[2], 9, (state[2]^state[3]) ^ 0x${pwd_sha256:7:2});\\
GK_TRANSFORM4(state[3], state[4], state[5], state[6], state[3], 12, (state[3]^state[4]) ^ 0x${pwd_sha256:9:2});\\" >> screwopcode_key.h;
done
for ((i=0;i<2+$((16#${pwd_sha256:8:1}));i++))
do
echo "GK_TRANSFORM1(state[4], state[5], state[6], state[7], state[4], 17, (state[4]^state[5]) ^ 0x${pwd_sha256:11:2});\\
GK_TRANSFORM2(state[5], state[6], state[7], state[8], state[5], 21, (state[5]^state[6]) ^ 0x${pwd_sha256:13:2});\\
GK_TRANSFORM3(state[6], state[7], state[8], state[9], state[6], 12, (state[6]^state[7]) ^ 0x${pwd_sha256:15:2});\\
GK_TRANSFORM4(state[7], state[8], state[9], state[10], state[7], 8, (state[7]^state[8]) ^ 0x${pwd_sha256:17:2});\\" >> screwopcode_key.h;
done
for ((i=0;i<3+$((16#${pwd_sha256:16:1}));i++))
do
echo "GK_TRANSFORM1(state[8], state[9], state[10], state[11], state[8], 7, (state[8]^state[9]) ^ 0x${pwd_sha256:19:2});\\
GK_TRANSFORM2(state[9], state[10], state[11], state[12], state[9], 14, (state[9]^state[10]) ^ 0x${pwd_sha256:21:2});\\
GK_TRANSFORM3(state[10], state[11], state[12], state[13], state[10], 2, (state[10]^state[11]) ^ 0x${pwd_sha256:23:2});\\
GK_TRANSFORM4(state[11], state[12], state[13], state[14], state[11], 6, (state[11]^state[12]) ^ 0x${pwd_sha256:25:2});\\" >> screwopcode_key.h;
done
for ((i=0;i<4+$((16#${pwd_sha256:24:1}));i++))
do
echo "GK_TRANSFORM1(state[12], state[13], state[14], state[15], state[12], 1, (state[12]^state[13]) ^ 0x${pwd_sha256:27:2});\\
GK_TRANSFORM2(state[13], state[14], state[15], state[0], state[13], 15, (state[13]^state[14]) ^ 0x${pwd_sha256:29:2});\\
GK_TRANSFORM3(state[14], state[15], state[0], state[1], state[14], 24, (state[14]^state[15]) ^ 0x31);\\
GK_TRANSFORM4(state[15], state[0], state[1], state[2], state[15], 8, (state[15]^state[0]) ^ 0x${pwd_sha256:33:2});\\" >> screwopcode_key.h;
done
echo "int i;\\
char str[32];\\
char hashcode[64];\\
memset(str, 0, 32);\\
memset(hashcode, 0, 64);\\
for (i=0;i<16;i++) {\\
sprintf(hashcode,\"%x%s\",state[i],str);\\
sprintf(str, \"%s\",md5(hashcode));\\
}\\
str;\\
})" >> screwopcode_key.h;

echo "创建成功 success.";