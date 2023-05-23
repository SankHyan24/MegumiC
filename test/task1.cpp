// #pragma warning(disable:4996)
// #include <stdio.h>

const int maxn = 10000 + 10;
int n, a[10010];

int ReadInt()
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

void PrintInt(int x)
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

int Median3(int a[], int Left, int Right)
{
	int Center = (Left + Right) / 2;
	int temp = 0;

	if (a[Left] > a[Center])
	{
		temp = a[Left];
		a[Left] = a[Center];
		a[Center] = temp;
	}
	if (a[Left] > a[Right])
	{
		temp = a[Left];
		a[Left] = a[Right];
		a[Right] = temp;
	}
	if (a[Center] > a[Right])
	{
		temp = a[Center];
		a[Center] = a[Right];
		a[Right] = temp;
	}

	temp = a[Center];
	a[Center] = a[Right - 1];
	a[Right - 1] = temp;

	return a[Right - 1];
}

void Quicksort(int a[], int Left, int Right)
{
	if (Left >= Right)
	{
		return;
	}
	if (Right - Left + 1 == 2)
	{
		if (a[Left] > a[Right])
		{
			int temp = a[Left];
			a[Left] = a[Right];
			a[Right] = temp;
		}
		return;
	}
	int L = Left;
	int R = Right;
	int Pivot = Median3(a, L, R);
	int i = Left;
	int j = Right - 1;
	while (1)
	{
		i = i + 1;
		while (a[i] < Pivot)
		{
			i = i + 1;
		}
		j = j - 1;
		while (a[j] > Pivot)
		{
			j = j - 1;
		}
		if (i < j)
		{
			int temp = a[i];
			a[i] = a[j];
			a[j] = temp;
		}
		else
		{
			break;
		}
	}

	int temp = a[i];
	a[i] = a[Right - 1];
	a[Right - 1] = temp;
	Quicksort(a, Left, i - 1);
	Quicksort(a, i + 1, Right);
}

int main()
{
	// freopen("./input/task1.1.in", "r", stdin);
	n = ReadInt();
	// scanf("%d", &n);
	int i = 1;
	while (i <= n)
	{
		a[i] = ReadInt();
		i = i + 1;
		// scanf("%d", &a[i]);
		// printf("a[%d] = %d\n", i, a[i]);
	}
	Quicksort(a, 1, n);
	i = 1;
	while (i <= n)
	{
		PrintInt(a[i]);
		i = i + 1;
		putch(10); // \n
				   // printf("a[%d] = %d\n", i, a[i]);
	}
	return 0;
}