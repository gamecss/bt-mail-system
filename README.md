Install compile require package: (from centos-base7 and epel7)
```sh
yum install -y gcc gcc-c++ openssl openssl-devel db4-devel ntpdate cyrus-sasl-md5 perl-GD perl-DBD-MySQL perl-GD perl-CPAN perl-CGI perl-CGI-Session cyrus-sasl-lib cyrus-sasl-plain cyrus-sasl cyrus-sasl-devel libtool-ltdl-devel libicu-devel texinfo automake autoconf expect libtool 
#Centos and epel's libtool didn't have libltdlc.la,and automake too old,so we later will remove they and manually compile libtool (or use prebuilt's libtool),see below
```

If you want to compile it yourself,first compile automake-1.16(.2):
```sh
cd automake-1.16.2
./bootstrap
./configure --prefix=/usr
make
yum remove -y automake #Remove system's default automake
make install
```
And second compile libtool-2.4.6:
```sh
cd libtool-2.4.6
./configure --prefix=/usr
make
yum remove -y libtool #Remove system's default libtool
make install
```
Or using prebuilt's libtool: (only for x86_64 centos7)
```sh
cd libtool-2.4.6/prebuilt-x86_64
yum remove -y libtool
cp -r * /usr
```