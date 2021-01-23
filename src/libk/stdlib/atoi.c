int atoi(const char str0[])
{
    char *str = (char *)str0;
    int i, n;
    n = 0;
    for (i = 0; str[i] >= '0' && str[i] <= '9'; ++i)
        n = 10 * n + (str[i] - '0');
    return n;
}