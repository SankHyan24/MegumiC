
function int ReadInt()
{
	int ch = getch();
	while ((ch > '9' || ch < '0') && ch != '-')
	{
		ch = getch();
	}
	int ans = 0;
	int flag = 1;
	if (ch == '-')
	{
		flag = -1;
		ch = getch();
	}
	while (ch >= '0' && ch <= '9')
	{
		ans = ans * 10 + ch - '0';
		ch = getch();
	}
	return ans * flag;
}

function int GetIntLength(int x)
{
	int ans = 1;
	if (x < 0)
	{
		x = -x;
		ans = ans + 1;
	}
	if (x >= 10)
	{
		ans = ans + GetIntLength(x / 10);
	}
	return ans;
}

function int PrintInt(int x)
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

int n1, m1, a1[35][35];
int n2, m2, a2[35][35];
int n3, m3, a3[35][35];

function int main()
{
	n1 = ReadInt();
	m1 = ReadInt();

	int i = 1;
	while (i <= n1)
	{
		int j = 1;
		while (j <= m1)
		{
			a1[i][j] = ReadInt();
			j = j + 1;
		}
		i = i + 1;
	}

	n2 = ReadInt();
	m2 = ReadInt();

	i = 1;
	while (i <= n2)
	{
		int j = 1;
		while (j <= m2)
		{
			a2[i][j] = ReadInt();
			j = j + 1;
		}
		i = i + 1;
	}

	if (m1 != n2)
	{
		putch('I');
		putch('n');
		putch('c');
		putch('o');
		putch('m');
		putch('p');
		putch('a');
		putch('t');
		putch('i');
		putch('b');
		putch('l');
		putch('e');
		putch(' ');
		putch('D');
		putch('i');
		putch('m');
		putch('e');
		putch('n');
		putch('s');
		putch('i');
		putch('o');
		putch('n');
		putch('s');
		putch(10);
		return 0;
	}

	n3 = n1;
	m3 = m2;
	i = 1;
	while (i <= n3)
	{
		int j = 1;
		while (j <= m3)
		{
			int k = 1;
			while (k <= m1)
			{
				a3[i][j] = a3[i][j] + a1[i][k] * a2[k][j];
				k = k + 1;
			}
			j = j + 1;
		}
		i = i + 1;
	}

	i = 1;
	while (i <= n3)
	{
		int j = 1;
		while (j <= m3)
		{
			int len = GetIntLength(a3[i][j]);
			int k = 1;
			while (k <= 10 - len)
			{
				putch(' ');
				k = k + 1;
			}
			PrintInt(a3[i][j]);
			j = j + 1;
		}
		i = i + 1;
		putch(10);
	}
	return 0;
}