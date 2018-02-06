#ifndef ________________________BITSTREAM___H
#define ________________________BITSTREAM___H

#include <vector>
#include <set>
#include <deque>
#include <string>
#include <map>
#include <list>
#include <string.h>
#include <sys/types.h>
#include <stdint.h>

/**
* Класс, используемый в InBitStream/OutBitStream для работы с битовым остатком
*/
struct BitsPosition
{
    size_t pos;
    size_t bitsRemainsInByte;
    BitsPosition(size_t p)
    {
        pos=p;
        bitsRemainsInByte=8;
    }
    void clear()
    {
        pos=0;
        bitsRemainsInByte=8;
    }
};
/**
*       Входной буфер с возможностью битового чтения/записи
*       Примечание: байтовые операции возможны только если текущий остаток битов равен нулю.
*/
class inBitStream
{
public:

private:
    BitsPosition m_pos;
    size_t m_size;
    const unsigned char *m_data;
    size_t m_BitsRemainsInByte;
public:
    ~inBitStream() {};
    inBitStream(const unsigned char *d, size_t size); // construct from an array
    inBitStream(const  char *d, size_t size); // construct from an array
    inBitStream(const  std::string&); // construct from string

    /// получить размер полного буфера без учета операций чтения
    size_t size() const
    {
        return m_size;
    }
    /// получить указатель на начало буфера без учета операций чтения
    const unsigned char *data() const
    {
        return m_data;
    }
    ///получить размер остатка непрочитанных данных
    size_t remains() const
    {
        return m_size - m_pos.pos;
    }
    /// узнать есть ли еще данные, доступные для чтения
    bool beforeEnd() const ;


public:
    /// распаковать остаток буфера. Байтовая операция. Байтовая операция
    std::string unpackRest();

    /// распаковать size байт и поместить в s. Если буфера недостаточно, то генерируется исключение
    /// \throw CommonError
    void unpack(std::string& s, size_t size);

    /// распаковать size bytes без генерации исключения. Байтовая операция
    /// \params dst     куда записать рапакованные данные
    /// \params size    размер данных
    /// \params success флаг успеха операции
    void unpack_nothrow(std::string& dst, size_t size, bool &success);

    /// получить из буфера длину в формате TLV Wimax. Байтовая операция
    unsigned int getTlvLength();

    /// получить 1 байт. Байтовая операция
    unsigned char get_8();

    /// получить 1 бит. Битовая операция
    int get_bit();
    /// получить текущий остаток битов в затронутом байте
    size_t paddingSize() const
    {
        return m_pos.bitsRemainsInByte;
    }
    /// получить целое с заданным количеством битов
    ///\param  bitcount  число бит
    int get_bits(int bitcount);

    /// получить 1 байт без генерации исключения
    unsigned char get_8_nothrow(bool &success);

    /// получить 2 байта в форматe Little Endian
    unsigned short get_16LE();
    /// получить 2 байта в форматe Little Endian без исключения
    unsigned short get_16LE_nothrow(bool &success);

    /// получить 2 байта в форматe Big Endian
    unsigned short get_16BE();
    /// получить 2 байта в форматe Big Endian без исключения
    unsigned short get_16BE_nothrow(bool &success);

    /// получить 4 байта в форматe Little Endian
    unsigned int get_32LE();

    /// получить 4 байта в форматe Little Endian без исключения
    unsigned int get_32LE_nothrow(bool &success);

    /// получить 4 байта в форматe Big Endian
    unsigned int get_32BE();
    /// получить 4 байта в форматe Big Endian без исключения
    unsigned int get_32BE_nothrow(bool &success);

    /// получить упакованный int
    unsigned long get_PN();

    /// получить упакованный int без исключения
    unsigned long get_PN_nothrow(bool &success);

    // получить строку с префиксом длины в виде PN
    std::string get_PSTR();
    // получить строку с префиксом длины в виде PN без исключения
    std::string get_PSTR_nothrow(bool &success);

    /// оператор вывода из буфера
    inBitStream& operator>>(int64_t&);
    inBitStream& operator>>(uint64_t&);
    inBitStream& operator>>(int32_t&);
    inBitStream& operator>>(uint32_t&);
    inBitStream& operator>>(int16_t&);
    inBitStream& operator>>(uint16_t&);
    inBitStream& operator>>(int8_t&);
    inBitStream& operator>>(uint8_t&);
    /// оператор вывода из буфера
    inBitStream& operator>>(std::string&);
    /// оператор вывода из буфера
    inBitStream& operator>>(bool&);
private:
    /// закрытый оператор для предотвращения конверсии типов
    inBitStream& operator>>(double&);
public:
};
/**
*       Выходной буфер с возможностью битового чтения/записи
*       Примечание: байтовые операции возможны только если текущий остаток битов равен нулю.
*/

class outBitStream
{
private:
    outBitStream(const outBitStream&);             // Not defined to prevent usage
    outBitStream& operator=(const outBitStream&);  // Not defined to prevent usage
public:
    unsigned char *buffer;
    size_t bufsize;
    BitsPosition m_pos;

    outBitStream();
    ~outBitStream();

    /// упаковать строку
    outBitStream& pack(const std::string& s);
    ///упаковать буфер
    outBitStream& pack(const char * s, size_t len);
    /// упаковать конст буфер
    outBitStream& pack(const unsigned char * s, size_t len);

    /// получить указатель на буфер конст
    const unsigned char *data()const
    {
        return buffer;
    }
    /// получить указатель на буфер
    unsigned char *const_data()const
    {
        return buffer;
    }
    /// получить текущий остаток битов в затронутом байте
    size_t paddingSize() const
    {
        return m_pos.bitsRemainsInByte;
    }
    /// получить результат как строку
    std::string asString() const;
    /// получить размер результата
    size_t size() const;
    void adjust(size_t n);
    void clear();
    /// записать число в формате Wimax TLV
    outBitStream& putTLVLength(unsigned long);
    /// записать байт
    outBitStream& put_8(unsigned char);
    ///записать биты из числа
    outBitStream& put_bits(unsigned int, int bitcout);
    /// записать число длиной в N бит в кодировке Big/Little endian
    outBitStream& put_16LE(unsigned short);
    /// записать число длиной в N бит в кодировке Big/Little endian
    outBitStream& put_16BE(unsigned short);
    /// записать число длиной в N бит в кодировке Big/Little endian
    outBitStream& put_32LE(unsigned int);
    /// записать число длиной в N бит в кодировке Big/Little endian
    outBitStream& put_32BE(unsigned int);
    /// записать число в упакованном формате
    outBitStream& put_PN(unsigned long);
    /// записать число с префиксом длины в упакованном формате
    outBitStream& put_PSTR(const std::string &);

    /// оператор ввода
    outBitStream& operator<<(const char*);

    outBitStream& operator<<(int64_t);
    outBitStream& operator<<(uint64_t);
    outBitStream& operator<<(int32_t);
    outBitStream& operator<<(uint32_t);
    outBitStream& operator<<(int16_t);
    outBitStream& operator<<(uint16_t);
    outBitStream& operator<<(int8_t);
    outBitStream& operator<<(uint8_t);
    /// оператор ввода
    outBitStream& operator<<(const std::string&);
    /// оператор ввода
    outBitStream& operator<<(const bool&);
private:
    /// закрытый оператор для предотвращения конверсии типов
    outBitStream& operator<<(const double&);
public:
private:
    outBitStream& put_8$(uint8_t c);
    outBitStream& put_8$(int8_t c);

    outBitStream& put_16LE$(uint16_t l);
    outBitStream& put_16BE$(uint16_t l);
    outBitStream& put_16LE$(int16_t l);
    outBitStream& put_16BE$(int16_t l);

    outBitStream& put_32LE$(uint32_t  l);
    outBitStream& put_32BE$(uint32_t l);

    outBitStream& put_32LE$(int32_t  l);
    outBitStream& put_32BE$(int32_t l);

    outBitStream& Pack$(const std::string& s );
    outBitStream& put_PN$(unsigned long N);


};

#endif
