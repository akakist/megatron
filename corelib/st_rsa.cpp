#include <string>

#include "st_rsa.h"
#include "IInstance.h"



REF_getter<refbuffer> st_AES::encrypt(const REF_getter<refbuffer> &buf)
{
    return impl->encrypt(buf);
}


REF_getter<refbuffer> st_AES::decrypt(const REF_getter<refbuffer> &buf)
{
    return impl->decrypt(buf);
}



//void st_AES::encrypt(unsigned char *buf, size_t size) {
//    impl->encrypt(buf,size);
//}
//void st_AES::decrypt(unsigned char *buf, size_t size) {
//    impl->decrypt(buf,size);
//}

std::string st_AES::generateRandomKey() {
    return impl->generateRandomKey();
}
st_AES::st_AES():impl(((I_ssl*)iUtils->queryIface(Ifaces::SSL))->aes_impl()) {}
void st_AES::init(const std::string &key) {
    impl->init(key);
}
st_AES::~st_AES() {

    delete impl;
}

void st_rsa::generate_key(const int b) {
    impl->generate_key(b);
}
void st_rsa::initFromPublicKey(const std::string &priv_key) {
    impl->initFromPublicKey(priv_key);
}
void st_rsa::initFromPrivateKey(const std::string &priv_key) {
    impl->initFromPrivateKey(priv_key);
}
std::string st_rsa::getPublicKey() {
    return impl->getPublicKey();
}
std::string st_rsa::getPrivateKey() {
    return impl->getPrivateKey();
}
std::string st_rsa::privateDecrypt(const std::string & m) {
    return impl->privateDecrypt(m);
}
std::string st_rsa::publicDecrypt(const std::string & m) {
    return impl->publicDecrypt(m);
}
std::string st_rsa::publicEncrypt(const std::string & m) {
    return impl->publicEncrypt(m);
}
std::string st_rsa::privateEncrypt(const std::string & m) {
    return impl->privateEncrypt(m);
}
int st_rsa::size() {
    return impl->size();
}
st_rsa::st_rsa(): impl(((I_ssl*)iUtils->queryIface(Ifaces::SSL))->rsa_impl())
{
}
st_rsa::~st_rsa()
{

    delete impl;
}

