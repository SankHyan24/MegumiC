
function int ReadInt()
{
    int ch = getch();
    while ((ch > 57 || ch < 48) && ch != 45) // 9 0 -
    {
        ch = getch();
    }
    int ans = 0;
    int flag = 1;

    while (ch >= 48 && ch <= 57)
    {
        ans = ans * 10 + ch - 48;
        ch = getch();
    }
    return ans * flag;
}

function int PrintInt(int x)
{
    if (x < 0)
    {
        putch(45); // '-'
        x = -x;
    }
    if (x >= 10)
    {
        PrintInt(x / 10);
    }
    putch(x % 10 + 48); // '0'
}

function int main()
{
    int arr[4][4];
    int i, j, k;
    i = 0;
    while (i < 4)
    {
        j = 0;
        while (j < 4)
        {
            PrintInt(i);
            putch(58);
            PrintInt(j);
            putch(58);
            k = i * 4 + j;
            arr[i][j] = k;
            PrintInt(k);
            putch(58);
            PrintInt(arr[i][j]);
            putch(10);
            j = j + 1;
        }
        i = i + 1;
    }
    i = 0;
    while (i < 4)
    {
        j = 0;
        while (j < 4)
        {
            PrintInt(i);
            putch(58);
            PrintInt(j);
            putch(58);
            PrintInt(arr[i][j]);
            putch(10);
            j = j + 1;
        }
        i = i + 1;
    }
    return 0;
}