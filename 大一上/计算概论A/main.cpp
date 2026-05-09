#include "Board.h"

Board cur;
vector<Board> memo; 

int player;
vi mem;

void Menu1() {
	system("cls");
	printf("操作指南：\n");
	printf("- 输入 S：开始一场新游戏\n");
	printf("- 输入 L：读取存档（存档 save.txt 位于代码相同目录），并继续游戏\n");
}

void Show_Start() {
	system("cls");
	printf("请选择先后手：\n");
	printf("- 输入 B：你选择执黑（先手）\n");
	printf("- 输入 W：你选择执白（后手）\n");
}

void Show_Board() {
	system("cls");
	printf("当前棋盘：\n");
	cur.Show();
	if(player==WHITE) printf("你是白方，请走棋\n");
	else printf("你是黑方，请走棋\n");
	printf("- 输入 Q：退出游戏\n");
	printf("- 输入 R：悔棋\n");
	printf("- 以 M a b c d e f 格式输入：执行一次将 (a,b) 移动到 (c,d) 并放置 (e,f) 障碍的操作\n");
	printf("棋盘字符分别表示：B 是黑色棋子；W 是白色棋子；. 是空格；# 是障碍\n");
}

void Show_Quit() {
	system("cls");
	printf("是否存档？：\n");
	printf("当前棋盘：\n");
	cur.Show();
	printf("- 输入 Y：存档至 save.txt\n");
	printf("- 输入 N：不存档\n");
}

void Save() {
	freopen("save.txt","w",stdout);
	cout<<player<<"\n"; // player's player
	cout<<sz(mem)<<"\n";
	for(int i:mem) cout<<i<<"\n";
	exit(0);
}

int main() {
	ios::sync_with_stdio(false);
	system("chcp 65001>nul"); 
	cur.Init();
	memo.pb(cur);
	Menu1();
	char ch; cin>>ch;
	while(ch!='S'&&ch!='L') {
		printf("错误：似乎输入了不该输入的字符\n");
		system("pause");
		Menu1(); cin>>ch;
	}
	if(ch=='S') {
		Show_Start();
		char ch1; cin>>ch1;
		while(ch1!='W'&&ch1!='B') {
			printf("错误：似乎输入了不该输入的字符\n");
			system("pause");
			Show_Start(); cin>>ch1;
		}
		if(ch1=='W') player=WHITE,cur.uct_player=BLACK;
		else player=BLACK,cur.uct_player=WHITE;
		if(player==WHITE) {
			cur.Move(0,5,6,5,3,2);
			memo.pb(cur);
			mem.pb(56532);
		}
	}
	else {
		ifstream fin;
		fin.open("save.txt");
		fin>>player;
		int n;
		fin>>n;
		FOR(i,1,n) {
			int mv;
			fin>>mv;
			cur.Move(mv/100000,mv/10000%10,mv/1000%10,mv/100%10,mv/10%10,mv%10);
			memo.pb(cur);
			mem.pb(mv);
		}
	}
	while(1) {
		Show_Board();
		cin>>ch;
		if(ch!='Q'&&ch!='R'&&ch!='M') {
			printf("错误：似乎输入了不该输入的字符\n");
			system("pause");
			continue;
		}
		if(ch=='Q') {
			Show_Quit(); cin>>ch;
			while(ch!='Y'&&ch!='N') {
				printf("错误：似乎输入了不该输入的字符\n");
				system("pause");
				Show_Quit(); cin>>ch;
			}
			if(ch=='Y') Save();
			else exit(0);
		}
		if(ch=='R') {
			if(sz(mem)<2) {
				printf("错误：这是第一步！\n");
				system("pause");
				continue;
			}
			mem.pop_back(),mem.pop_back();
			memo.pop_back(),memo.pop_back();
			cur=memo.back();
			
		}
		if(ch=='M') {
			int sx=read()-1,sy=read()-1,tx=read()-1,ty=read()-1,bx=read()-1,by=read()-1;
			if(!cur.Check_Move(sx,sy,tx,ty,bx,by,1)) continue;
			cur.Move(sx,sy,tx,ty,bx,by);
			mem.pb(by+bx*10+ty*100+tx*1000+sy*10000+sx*100000);
			memo.pb(cur);
			int tt=0;
			if(tt=cur.Board_Judge()) {
				if(tt==3-player||tt==3) {
					puts("你赢了！");
					// cerr<<tt<<"\n";
					cur.Show(); //Save();
					system("pause");
					return 0;
				}
			}
			db st=clock();
			Board tmp; tmp.Init();
			tmp.uct_player=3-player;
			for(int i:mem) tmp.Move(i/100000,i/10000%10,i/1000%10,i/100%10,i/10%10,i%10);
			while((db)clock()-st<0.96*CLOCKS_PER_SEC) tmp.Search();
			int id=0; int t=sz(tmp.Son); 
			FOR(i,1,t-1) {
				if(tmp.Son[i]->visits>tmp.Son[id]->visits) id=i;
				else if(tmp.Son[i]->visits==tmp.Son[id]->visits&&tmp.Son[i]->score>tmp.Son[id]->score)
					id=i;
			}
			int sol=tmp.Son[id]->fat_move;
			if(!cur.Check_Move(sol/100000,sol/10000%10,sol/1000%10,sol/100%10,sol/10%10,sol%10,1)) {
				cerr<<"???????";
				exit(0);
			}
			cur.Move(sol/100000,sol/10000%10,sol/1000%10,sol/100%10,sol/10%10,sol%10);
			memo.pb(cur);
			mem.pb(sol);
			if(tt=cur.Board_Judge()) {
				if(tt==player||tt==3) {
					puts("你输了！");
					cur.Show(); //Save();
					system("pause");
					return 0;
				}
			}
		}
	}
}