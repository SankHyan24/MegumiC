

function int print_num(int c)
{
    int a[10];
    int i = 0;
    while (c)
    {
        a[i++] = c % 10;
        c /= 10;
    }
    while (i)
    {
        i = i - 1;
        putch(a[] + '0');
    }
    return 0;
}
function int main()
{
    print_num(12);
    return 0;
}