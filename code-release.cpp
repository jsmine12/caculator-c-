#include <graphics.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
//button wide is 75      75*4=300
//button height is 75
//screen frame height is 75*3
int heigtBu = 75;
int wideBu = 105;
int frameheight = heigtBu * 8, framewide = wideBu * 4;
int BorderLine = heigtBu * 3;
int DisplayCharWide = wideBu / 2; //DisplayCharWide* (2 * 1.0 / 5)
int DisplayCharheigt = heigtBu / 2; //DisplayCharheigt* (6 * 1.0 / 7)* i
int ActualCharWide = DisplayCharWide * (2 * 1.0 / 5);//max is 20
int ActualCharheigt = DisplayCharheigt * (6 * 1.0 / 7);//max is 7
const double def = -10000000000;
const char epty = '&';
char ArryForput[7][20];
int check(char Stacktop, char value); //judge priority
int IfFuHao(char a);//distinguish - is jianhao or fuhao //4-4 then there have two figure one is 4 another is 4//-4+4 then there have two figure one is -4 another is 4
double work(double x, double y, char a, bool* UnexpectedExist); //caculate
void showButton();//show layout
char updateWithInput();//鼠标键盘交互
void showDisplay();
void showExp(vector<char> exp);
int main() {
	initgraph(framewide, frameheight);
	HWND window;
	window = GetHWnd();
	SetWindowText(window, L"计算器");//design window tittle
	bool FigureTime = false;
	while (1) {
		if (FigureTime == true) {
			char ini = _getch();
			while (ini!='z') {
				ini = _getch();;
			}
		}
		FigureTime = true;
		cleardevice();
		showDisplay();//visual output
		settextstyle(ActualCharheigt, ActualCharWide, L"行楷");
		vector<char> exp;
		while (1) {
			char c;
			//scanf("%c", &c);
			c = updateWithInput();
			if (c == 'D') {
				cleardevice();
				showDisplay();
				if(exp.size()!=0)exp.pop_back();
				showExp(exp);
			}
			if (c == '\n')break;
			if ((c >= '0' && c <= '9') || c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '=' || c == '(' || c == ')' || c == '.') {
				exp.push_back(c);
				//if(exp.size()>140);//gererally the situation will not happen
				showExp(exp);
			}
		}
		vector<double> DouExp;
		vector<char> CharExp;
		vector<bool> BoArry; //if value is char and BoStack is true;
		for (int i = 0; i < exp.size(); i++) {
			//if((exp[i]>='0'&&exp[i]<='9')||exp[i]=='.'||exp[i]=='-'){
			if ((exp[i] >= '0' && exp[i] <= '9') || (exp[i] == '-' && (i - 1 == -1 || IfFuHao(exp[i - 1])))) {//the value first detected surely is '0'-'9' and may eixst -3.14
				double value = 0;
				double Dval = 1;
				int flag = 1;
				int IFminus = 0;

				while ((exp[i] >= '0' && exp[i] <= '9') || exp[i] == '.' || (exp[i] == '-' && (i - 1 == -1 || IfFuHao(exp[i - 1])))) {//- must appear the front of . 
					if (exp[i] == '-')IFminus = 1;
					if ((exp[i] >= '0' && exp[i] <= '9') && flag == 1) {

						value = value * 10 + exp[i] - '0';
						//cout<<"int value is "<<value<<endl;
					}
					if (exp[i] == '.') {
						//cout<<"finding . \n";
						flag = 0;
					}
					if (flag == 0 && exp[i] != '.') {
						Dval = Dval * 0.1;
						value = value + Dval * (exp[i] - '0');
						//cout<<"double value is "<<value<<endl;
					}
					i++;
					if (i >= exp.size())break;
				}
				if (IFminus == 0)DouExp.push_back(value);
				else DouExp.push_back(-1 * value);
				CharExp.push_back(epty);
				BoArry.push_back(false);
				i--;//adjust i , element in index i-- is char 
			}
			else {
				DouExp.push_back(def);//push def
				CharExp.push_back(exp[i]);
				BoArry.push_back(true);
			}
		}//read string 
		vector<char> OperatorStack;

		vector<char> rChStack;
		vector<double> rDouStack;//
		vector<bool> jStack;// if value is char and jStack is true;
		for (int i = 0; i != BoArry.size(); i++) {
			if (BoArry[i] == false) {
				rDouStack.push_back(DouExp[i]);
				jStack.push_back(false);
				rChStack.push_back(epty);
			}//typeElement is '0'-'9' then directly push into reStack
			else {
				if (OperatorStack.size() == 0) {
					OperatorStack.push_back(CharExp[i]);
				}
				else {
					if (CharExp[i] == ')') {// at the front of the ) must exist ( to match with )
						int Stacktop = OperatorStack.size() - 1;
						while (OperatorStack[Stacktop] != '(') {
							char TopValue = OperatorStack[Stacktop];
							OperatorStack.pop_back();//pop

							rDouStack.push_back(def);
							jStack.push_back(true);
							rChStack.push_back(TopValue);

							Stacktop--;
						}
						OperatorStack.pop_back();//pop (
					}//find closest ( then pop all char between ( and )
					else {
						int Stacktop = OperatorStack.size() - 1;
						while (OperatorStack.size() != 0 && check(OperatorStack[Stacktop], CharExp[i]) == 1) {//if OperatorStack.size()!=0 and check
							rDouStack.push_back(def);
							jStack.push_back(true);
							rChStack.push_back(OperatorStack[Stacktop]);
							OperatorStack.pop_back();
							Stacktop--;
						}
						OperatorStack.push_back(CharExp[i]);
					}
				}
			}
		}//use a stack to turn an middle expression into a rear expression     
		if (OperatorStack.size() != 0) {
			//int Top=OperatorStack.size()-1;
			while (OperatorStack.size() != 0) {
				char Top = *(OperatorStack.end() - 1);
				OperatorStack.pop_back();

				rDouStack.push_back(def);
				jStack.push_back(true);
				rChStack.push_back(Top);
			}
		}
		vector<double> EndValue;
		bool UnexpectedExist = 0;
		for (int i = 0; i < jStack.size(); i++) {
			if (jStack[i] == true) {
				double Top = *(EndValue.end() - 1);
				double LastTop = *(EndValue.end() - 2);
				EndValue.pop_back();
				*(EndValue.end() - 1) = work(LastTop, Top, rChStack[i], &UnexpectedExist);
			}
			else {
				EndValue.push_back(rDouStack[i]);
			}
		}
		exp.push_back('=');
		string fault = "Error Zero as a divisor";
		if (UnexpectedExist == 0) {
			bool IFint = false;
			int integer = EndValue[0];
			if ((EndValue[0] - integer) < 1e-5)IFint = true;
			string s;
			if (IFint == false) {
				char str[100];
				sprintf_s(str, "%f", EndValue[0]);
				s = str;
				for (int i = 0; i < s.size(); i++) {
					exp.push_back(s[i]);
				}
			}
			else {
				char str[100];
				sprintf_s(str, "%d", integer);
				s = str;
				for (int i = 0; i < s.size(); i++) {
					exp.push_back(s[i]);
				}
			}
		}
		else {
			for (int i = 0;i<fault.size(); i++) {
				exp.push_back(' ');
				exp.push_back(fault[i]);
			}
		}
		//if(exp.size()>140);//gererally the situation will not happen
		showExp(exp);
	}
	//_getch();
	//closegraph();
	return 0;
}
int check(char Stacktop, char value) //judge priority
{
	if (Stacktop == '(')return 0;//( has lowest priority it must not be pop
	if (Stacktop == '^') {
		if (value == '(') {
			return 0;
		}
		else return 1;
	}//^ has top priority it must be pop unless value is (
	if (Stacktop == '+' || Stacktop == '-') {
		if (value == '+' || value == '-')return 1;
		else return 0;
	}
	if (Stacktop == '*' || Stacktop == '/') {
		if (value == '+' || value == '-' || value == '*' || value == '/')return 1;
		else return 0;
	}
}
int IfFuHao(char a) {//distinguish - is jianhao or fuhao //4-4 then there have two figure one is 4 another is 4//-4+4 then there have two figure one is -4 another is 4
	if (a == '(' || a == '+' || a == '-' || a == '*' || a == '/' || a == '^')return 1;
	else return 0;//a is ) or '0'-'9'
}
double work(double x, double y, char a, bool* UnexpectedExist) //caculate
{
	if (a == '+')return x + y;
	if (a == '-')return x - y;
	if (a == '*')return x * y;
	if (a == '/') {
		if (y == 0) {
			*UnexpectedExist = 1;
			return def;
		}
		else return x * 1.0 / y;
	}
	if (a == '^')return pow(x, y);
}
void showButton() {//show layout
	settextcolor(BLACK);
	settextstyle(heigtBu * (6 * 1.0 / 7), wideBu * (2 * 1.0 / 5), L"行楷");//80 is char's height 60 is wide
	{
		outtextxy(wideBu * 1.0 / 3, double(BorderLine * 1.05), TEXT("D"));//datatype of outtextxy Chanshu both is int//outtextxy(int,int,string)
		outtextxy(wideBu * 1.0 / 3 + wideBu, BorderLine * 1.05, TEXT("("));
		outtextxy(wideBu * 1.0 / 3 + wideBu * 2, BorderLine * 1.05, TEXT(")"));
		outtextxy(wideBu * 1.0 / 3 + wideBu * 3, BorderLine * 1.05, TEXT("/"));
	}
	{
		outtextxy(wideBu * 1.0 / 3, double(BorderLine * 1.05) + heigtBu, TEXT("7"));//datatype of outtextxy Chanshu both is int//outtextxy(int,int,string)
		outtextxy(wideBu * 1.0 / 3 + wideBu, BorderLine * 1.05 + heigtBu, TEXT("8"));
		outtextxy(wideBu * 1.0 / 3 + wideBu * 2, BorderLine * 1.05 + heigtBu, TEXT("9"));
		outtextxy(wideBu * 1.0 / 3 + wideBu * 3, BorderLine * 1.05 + heigtBu, TEXT("*"));
	}
	{
		outtextxy(wideBu * 1.0 / 3, double(BorderLine * 1.05) + heigtBu * 2, TEXT("4"));//datatype of outtextxy Chanshu both is int//outtextxy(int,int,string)
		outtextxy(wideBu * 1.0 / 3 + wideBu, BorderLine * 1.05 + heigtBu * 2, TEXT("5"));
		outtextxy(wideBu * 1.0 / 3 + wideBu * 2, BorderLine * 1.05 + heigtBu * 2, TEXT("6"));
		outtextxy(wideBu * 1.0 / 3 + wideBu * 3, BorderLine * 1.05 + heigtBu * 2, TEXT("-"));
	}
	{
		outtextxy(wideBu * 1.0 / 3, double(BorderLine * 1.05) + heigtBu * 3, TEXT("1"));//datatype of outtextxy Chanshu both is int//outtextxy(int,int,string)
		outtextxy(wideBu * 1.0 / 3 + wideBu, BorderLine * 1.05 + heigtBu * 3, TEXT("2"));
		outtextxy(wideBu * 1.0 / 3 + wideBu * 2, BorderLine * 1.05 + heigtBu * 3, TEXT("3"));
		outtextxy(wideBu * 1.0 / 3 + wideBu * 3, BorderLine * 1.05 + heigtBu * 3, TEXT("+"));
	}
	{
		outtextxy(wideBu * 1.0 / 3, double(BorderLine * 1.05) + heigtBu * 4, TEXT("^"));//datatype of outtextxy Chanshu both is int//outtextxy(int,int,string)
		outtextxy(wideBu * 1.0 / 3 + wideBu, BorderLine * 1.05 + heigtBu * 4, TEXT("0"));
		outtextxy(wideBu * 1.0 / 3 + wideBu * 2, BorderLine * 1.05 + heigtBu * 4, TEXT("."));
		outtextxy(wideBu * 1.0 / 3 + wideBu * 3, BorderLine * 1.05 + heigtBu * 4, TEXT("="));
	}
	{
		outtextxy(wideBu * 1.0 / 3, double(BorderLine * 1.05) + heigtBu * 5, TEXT("7"));//datatype of outtextxy Chanshu both is int//outtextxy(int,int,string)
		outtextxy(wideBu * 1.0 / 3 + wideBu, BorderLine * 1.05 + heigtBu * 5, TEXT("8"));
		outtextxy(wideBu * 1.0 / 3 + wideBu * 2, BorderLine * 1.05 + heigtBu * 5, TEXT("9"));
		outtextxy(wideBu * 1.0 / 3 + wideBu * 3, BorderLine * 1.05 + heigtBu * 5, TEXT("*"));
	}
}
char updateWithInput() {//游戏输入 
	MOUSEMSG    msg;
	if (MouseHit()) {
		msg = GetMouseMsg();
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > wideBu * 0 && msg.x< wideBu * 1 && msg.y> BorderLine + heigtBu * 0 && msg.y < BorderLine + heigtBu * 1)) {
			return 'D';//delete one element
		}//D
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > wideBu * 1 && msg.x< wideBu * 2 && msg.y> BorderLine + heigtBu * 0 && msg.y < BorderLine + heigtBu * 1)) {
			return '(';
		}//(
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > wideBu * 2 && msg.x < wideBu * 3 && msg.y > BorderLine + heigtBu * 0 && msg.y < BorderLine + heigtBu * 1)) {
			return ')';
		}// )
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > wideBu * 3 && msg.x < wideBu * 4 && msg.y > BorderLine + heigtBu * 0 && msg.y < BorderLine + heigtBu * 1)) {
			return '/';
		}//  /

		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > wideBu * 0 && msg.x< wideBu * 1 && msg.y> BorderLine + heigtBu * 1 && msg.y < BorderLine + heigtBu * 2)) {
			return '7';
		}//7
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > wideBu * 1 && msg.x < wideBu * 2 && msg.y> BorderLine + heigtBu * 1 && msg.y < BorderLine + heigtBu * 2)) {
			return '8';
		}//8 no problem
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > wideBu * 2 && msg.x < wideBu * 3 && msg.y> BorderLine + heigtBu * 1 && msg.y < BorderLine + heigtBu * 2)) {
			return '9';
		}// 9
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > wideBu * 3 && msg.x < wideBu * 4 && msg.y> BorderLine + heigtBu * 1 && msg.y < BorderLine + heigtBu * 2)) {
			return '*';
		}//  *

		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > 0 + wideBu * 0 && msg.x < wideBu * 1 && msg.y> BorderLine + heigtBu * 2 && msg.y < BorderLine + heigtBu * 3)) {
			return '4';
		}//  4
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > 0 + wideBu * 1 && msg.x < wideBu * 2 && msg.y> BorderLine + heigtBu * 2 && msg.y < BorderLine + heigtBu * 3)) {
			return '5';
		}//  5
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > 0 + wideBu * 2 && msg.x < wideBu * 3 && msg.y> BorderLine + heigtBu * 2 && msg.y < BorderLine + heigtBu * 3)) {
			return '6';
		}//  6
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > 0 + wideBu * 3 && msg.x < wideBu * 4 && msg.y> BorderLine + heigtBu * 2 && msg.y < BorderLine + heigtBu * 3)) {
			return '-';
		}//  -

		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > 0 + wideBu * 0 && msg.x < wideBu * 1 && msg.y> BorderLine + heigtBu * 3 && msg.y < BorderLine + heigtBu * 4)) {
			return '1';
		}//  1
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > 0 + wideBu * 1 && msg.x < wideBu * 2 && msg.y> BorderLine + heigtBu * 3 && msg.y < BorderLine + heigtBu * 4)) {
			return '2';
		}//  2
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > 0 + wideBu * 2 && msg.x < wideBu * 3 && msg.y> BorderLine + heigtBu * 3 && msg.y < BorderLine + heigtBu * 4)) {
			return '3';
		}//  3
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > 0 + wideBu * 3 && msg.x < wideBu * 4 && msg.y> BorderLine + heigtBu * 3 && msg.y < BorderLine + heigtBu * 4)) {
			return '+';
		}//  +

		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > 0 + wideBu * 0 && msg.x < wideBu * 1 && msg.y> BorderLine + heigtBu * 4 && msg.y < BorderLine + heigtBu * 5)) {
			return '^';
		}//  ^
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > 0 + wideBu * 1 && msg.x < wideBu * 2 && msg.y> BorderLine + heigtBu * 4 && msg.y < BorderLine + heigtBu * 5)) {
			return '0';
		}//  0
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > 0 + wideBu * 2 && msg.x < wideBu * 3 && msg.y> BorderLine + heigtBu * 4 && msg.y < BorderLine + heigtBu * 5)) {
			return '.';
		} //  .
		if (msg.uMsg == WM_LBUTTONDOWN && (msg.x > 0 + wideBu * 3 && msg.x < wideBu * 4 && msg.y> BorderLine + heigtBu * 4 && msg.y < BorderLine + heigtBu * 5)) {
			return '\n';
		}//  =
		else return epty;
	}
	else return epty;
}//鼠标键盘交互
void showDisplay() {
	setbkcolor(WHITE);
	cleardevice();
	setlinecolor(BLACK);
	line(0, BorderLine, framewide, BorderLine);//Display frame is twice the size of the buttton;
	for (int i = BorderLine, j = 0; i <= frameheight; i += heigtBu, j += wideBu) {
		line(0, i, 420, i);
		if (i != frameheight)line(j, BorderLine, j, 600);
	}
	showButton();
}
void showExp(vector<char> exp) {
	settextstyle(ActualCharheigt, ActualCharWide, L"行楷");
	int row = 6, line = 19;
	for (int i = exp.size() - 1; i >= 0; i--) {
		outtextxy(line * ActualCharWide, row * ActualCharheigt, exp[i]);
		line--;
		if (line == -1) {
			line = 19;
			row--;
		}
	}
}