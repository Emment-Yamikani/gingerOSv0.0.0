unsigned char inb(unsigned short port)
{
    unsigned char data;
    __asm__ __volatile__("inb %1, %0"
                         : "=a"(data)
                         : "dN"(port));
    return data;
}

void outb(unsigned short port, unsigned char data)
{
    __asm__ __volatile__("outb %1, %0"
                         :
                         : "dN"(port), "a"(data));
}