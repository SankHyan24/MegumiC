// int need[110][8][8];
int need1[8][9][10];

static int PrintInt(int x)
{
    if (x < 0)
    {
        putch('-');
        x = -x;
    }
    if (x >= 10)
    {
        PrintInt(x / 10);
    }
    putch(x % 10 + '0');
}

static int main()
{
    int i = 1;
    int j = 1;
    int k = 1;
    while (i < 8)
    {
        j = 1;
        while (j < 9)
        {
            k = 1;
            while (k < 10)
            {
                // PrintInt(i * 100 + j * 10 + k);
                // putch(' ');
                need1[i][j][k] = i * 100 + j * 10 + k;
                k = k + 1;
            }
            j = j + 1;
        }
        i = i + 1;
    }
    putch(10);
    i = 1;
    while (i < 8)
    {
        j = 1;
        while (j < 9)
        {
            k = 1;
            while (k < 10)
            {
                // PrintInt(i * 100 + j * 10 + k);
                PrintInt(need1[i][j][k]);
                putch(' ');
                k = k + 1;
            }
            j = j + 1;
        }
        putch(10);
        i = i + 1;
    }
}