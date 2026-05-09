#include "Head.h"

class Board {
private:
	int mp[8][8],chess[3][4];
public:
	int player,turn;
	bool State;
	void Show();
	void Init();
	bool In(int x,int y);
	bool Check_Move(int sx,int sy,int ex,int ey,int bx,int by,bool op);
	void Move(int sx,int sy,int ex,int ey,int bx,int by,int col);
	void Regret(int sx,int sy,int ex,int ey,int bx,int by,int col);
	bool Search_Move(int x,int y);
	int Board_Judge();
	void NextTurn();
	// for UCT
	int visits,Solcnt,uct_player,fat_move;
	db score;
	vector<Board*> Son;
	set<int> Used;
	int calc_Move(int x,int y);
	void calc_Queenmove(int col,int dist[8][8]);
	void calc_Kingmove(int col,int dist[8][8]);
	void calcSol();
	int randSol(Board *x,int col);
	bool noSon();
	bool isLeaf();
	void copy(Board fa);
	db pure_eval();
	db eval();
	Board* Bestfornow();
	Board* expand();
	void Search();
	void UCTInit();
};

void Board::Init() {
	FOR(i,0,7) FOR(j,0,7) mp[i][j]=EMPTY;
	mp[0][2]=mp[2][0]=mp[0][5]=mp[2][7]=BLACK;
	mp[5][0]=mp[7][2]=mp[5][7]=mp[7][5]=WHITE;
	chess[1][0]=2,chess[1][1]=20,chess[1][2]=5,chess[1][3]=27;
	chess[2][0]=50,chess[2][1]=72,chess[2][2]=57,chess[2][3]=75;
	player=BLACK;
}

void Board::UCTInit() {
	visits=Solcnt=fat_move=0; uct_player=player;
	score=0;
	Son.clear();
	Used.clear();
}

void Board::Show() {
	// system("cls");
	cerr<<"\t";
	FOR(i,1,8) cerr<<i<<"\t";
	cerr<<"\t"<<"\n";
	FOR(i,0,7) {
		cerr<<i+1<<"\t"; 
		FOR(j,0,7) {
			if(mp[i][j]==EMPTY) cerr<<'.'<<"\t";
			if(mp[i][j]==BLACK) cerr<<'B'<<"\t";
			if(mp[i][j]==WHITE) cerr<<'W'<<"\t";
			if(mp[i][j]==BLOCK) cerr<<'#'<<"\t";
		}
		cerr<<"\n";
	}
}

bool Board::In(int x,int y) {
	if(x>=0&&x<=SIZE-1&&y>=0&&y<=SIZE-1) return 1;
	return 0;
}

bool Board::Check_Move(int sx,int sy,int ex,int ey,int bx,int by,bool op=0) {
	if(!In(sx,sy)||!In(ex,ey)||!In(bx,by)) {
		if(op) cout<<"错误：坐标出界！请检查你的操作\n",system("pause");
		return 0;
	}
	set<int> it; int x=sx,y=sy;
	FOR(i,0,7) {
		int nx=x+dx[i],ny=y+dy[i];
		while(In(nx,ny)&&mp[nx][ny]==EMPTY) {
			swap(mp[nx][ny],mp[x][y]);
			FOR(j,0,7) {
				int zx=nx+dx[j],zy=ny+dy[j];
				while(In(zx,zy)&&mp[zx][zy]==EMPTY) {
					it.insert(zy+zx*10+ny*100+nx*1000+y*10000+x*100000);
					zx+=dx[j],zy+=dy[j];
				}
			}
			swap(mp[nx][ny],mp[x][y]);
			nx+=dx[i],ny+=dy[i];
		}
	}
	if(!it.count(by+bx*10+ey*100+ex*1000+sy*10000+sx*100000)) {
		if(op) printf("错误：不在合法操作中！\n"),system("pause");
		return 0;
	}
	return 1;
}

void Board::Move(int sx,int sy,int ex,int ey,int bx,int by,int col=-1) {
	if(col==-1) col=player;
	int it=0;
	FOR(i,0,3) if(chess[col][i]/10==sx&&chess[col][i]%10==sy)
		it=i;
	mp[sx][sy]=EMPTY,mp[ex][ey]=col;
	chess[col][it]=ex*10+ey;
	mp[bx][by]=BLOCK;
	turn++,player=3-player;
}

void Board::Regret(int sx,int sy,int ex,int ey,int bx,int by,int col=-1) {
	if(col==-1) col=player;
	// cerr<<"Regret "<<sx<<" "<<sy<<" "<<ex<<" "<<ey<<" "<<bx<<" "<<by<<" "<<col<<"\n";
	int it=0;
	FOR(i,0,3) if(chess[col][i]/10==sx&&chess[col][i]%10==sy)
		it=i;
	mp[sx][sy]=col,mp[ex][ey]=EMPTY;
	chess[col][it]=sx*10+sy;
	mp[bx][by]=EMPTY;
	player=3-player,turn--;
}

bool Board::Search_Move(int x,int y) {
	FOR(i,0,7) {
		int nx=x+dx[i],ny=y+dy[i];
		if(In(nx,ny)&&mp[nx][ny]==EMPTY) return 1;
	}
	return 0;
}

int Board::Board_Judge() {
	bool fl1=0,fl2=0;
	// cerr<<"JUDGE\n";
	FOR(i,0,3) fl1|=Search_Move(chess[1][i]/10,chess[1][i]%10);
	FOR(i,0,3) fl2|=Search_Move(chess[2][i]/10,chess[2][i]%10);
	// cerr<<fl1<<" "<<fl2<<"\n";
	if(fl1==0||fl2==0) { State=1; return ((fl1==0)?BLACK:0)+((fl2==0)?WHITE:0); }
	return 0;
}

int Board::calc_Move(int x,int y) {
	int ret=0;
	FOR(i,0,7) {
		int nx=x+dx[i],ny=y+dy[i];
		while(In(nx,ny)&&mp[nx][ny]==EMPTY) {
			swap(mp[nx][ny],mp[x][y]);
			FOR(j,0,7) {
				int zx=nx+dx[j],zy=ny+dy[j];
				while(In(zx,zy)&&mp[zx][zy]==EMPTY) {
					ret++;
					zx+=dx[j],zy+=dy[j];
				}
			}
			swap(mp[nx][ny],mp[x][y]);
			nx+=dx[i],ny+=dy[i];
		}
	}
	return ret;
}

void Board::calcSol() {
	int col=player;
	FOR(i,0,3) Solcnt+=calc_Move(chess[col][i]/10,chess[col][i]%10);
}

bool Board::noSon() {
	if(!Solcnt) calcSol();
	return Solcnt<=sz(Son);
}

bool Board::isLeaf() {
	// cerr<<"In isLeaf\n";
	if(State) return 1;
	return Board_Judge();
}
Board* Board::Bestfornow() {
	// cerr<<"In Best\n";
	Board* best=nullptr;
	db ret=-inf;
	for(Board* it:Son) {
		// if(it->visits==0) return it;
		db s=(player==uct_player?it->score/it->visits:-it->score/it->visits)+
					  C*sqrt(log(visits+0.0001)/it->visits);
		if(s>ret) ret=s,best=it;
	}
	// cerr<<"Out Best\n";
	return best;
}

void Board::copy(Board fa) {
	FOR(i,0,7) FOR(j,0,7) mp[i][j]=fa.mp[i][j];
	player=fa.player;
	uct_player=fa.uct_player;
	FOR(i,1,2) FOR(j,0,3) chess[i][j]=fa.chess[i][j];
	turn=fa.turn;
}

mt19937 rng(time(0));
int Board::randSol(Board *x,int player) {
	int p4[4],p81[8],p82[8];
	FOR(i,0,3) p4[i]=i;
	FOR(i,0,7) p81[i]=i,p82[i]=i;
	shuffle(p4,p4+4,rng),shuffle(p81,p81+8,rng),shuffle(p82,p82+8,rng);
	// cerr<<"?"<<"\n";
	FOR(i,0,3) {
		int sx=chess[player][i]/10,sy=chess[player][i]%10;
		// cerr<<sx<<" "<<sy<<"\n";
		FOR(j,0,7) {
			// cerr<<p81[j]<<"\n";
			// cerr<<dx[p81[j]]<<"\n";
			int nx=sx+dx[p81[j]],ny=sy+dy[p81[j]];
			// cerr<<nx<<" "<<ny<<"\n";
			vi tmp;
			while(In(nx,ny)&&mp[nx][ny]==EMPTY) {
				swap(mp[nx][ny],mp[sx][sy]);
				FOR(k,0,7) {
					int zx=nx+dx[p82[k]],zy=ny+dy[p82[k]];
					while(In(zx,zy)&&mp[zx][zy]==EMPTY) {
						int tmpsol=sx*100000+sy*10000+nx*1000+ny*100+zx*10+zy;
						if(x==nullptr||x->Used.find(tmpsol)==x->Used.end()) tmp.pb(tmpsol);
						zx+=dx[p82[k]],zy+=dy[p82[k]];
					}
				}
				swap(mp[nx][ny],mp[sx][sy]);
				nx+=dx[p81[j]],ny+=dy[p81[j]];
			}
			if(sz(tmp)) return tmp[rng()%sz(tmp)];
		}
	}
	return -1;
}

void Board::calc_Queenmove(int col,int dist[8][8]) {
	queue<pii> qu;
	FOR(i,0,7) FOR(j,0,7) dist[i][j]=9*9;
	FOR(i,0,3) { int x=chess[col][i]/10,y=chess[col][i]%10; dist[x][y]=0; qu.push(x,y); }
	while(sz(qu)) {
		auto [x,y]=qu.front(); qu.pop();
		FOR(i,0,7) {
			int nx=x+dx[i],ny=y+dy[i];
			while(In(nx,ny)&&mp[nx][ny]==EMPTY) {
				if(dist[nx][ny]>dist[x][y]+1)
					dist[nx][ny]=dist[x][y]+1,qu.push(nx,ny);
				nx+=dx[i],ny+=dy[i];
			}
		}
	}
}

void Board::calc_Kingmove(int col,int dist[8][8]) {
	queue<pii> qu;
	FOR(i,0,7) FOR(j,0,7) dist[i][j]=9*9;
	FOR(i,0,3) { int x=chess[col][i]/10,y=chess[col][i]%10; dist[x][y]=0; qu.push(x,y); }
	while(sz(qu)) {
		auto [x,y]=qu.front(); qu.pop();
		FOR(i,0,7) {
			int nx=x+dx[i],ny=y+dy[i];
			if(In(nx,ny)&&dist[nx][ny]>dist[x][y]+1)
				dist[nx][ny]=dist[x][y]+1,qu.push(nx,ny);
		}
	}
}

int pow2(int x) {
	if(x>=32) return INT_MAX;
	return (1<<x);
}

db Board::pure_eval() {
	int Queendist1[8][8],Queendist2[8][8],Kingdist1[8][8],Kingdist2[8][8];
	// cerr<<"calc Queenmove\n";
	calc_Queenmove(BLACK,Queendist1),calc_Queenmove(WHITE,Queendist2);
	// cerr<<"calc Kingmove\n";
	calc_Kingmove(BLACK,Kingdist1),calc_Kingmove(WHITE,Kingdist2);
	int movecnt1=0,movecnt2=0;
	db tq=0,tk=0,p1=0,p2=0,m=0;
	FOR(i,0,7) FOR(j,0,7) {
		if(Queendist1[i][j]==Queendist2[i][j]&&Queendist1[i][j]!=9*9) tq+=(player==BLACK?0.25:-0.25);
		else if(Queendist1[i][j]<Queendist2[i][j]) tq+=1;
		else tq-=1;
		if(Kingdist1[i][j]==Kingdist2[i][j]&&Kingdist1[i][j]!=9*9) tk+=(player==BLACK?0.25:-0.25);
		else if(Kingdist1[i][j]<Kingdist2[i][j]) tk+=1;
		else tk-=1;
		p1+=2*(1.0/(pow2(Queendist1[i][j]))-1.0/pow2(Queendist1[i][j]));
        p2+=min(1.0,max(-1.0,(Kingdist1[i][j]-Kingdist2[i][j])/6.0));
	}
	FOR(i,0,3) movecnt1+=calc_Move(chess[1][i]/10,chess[1][i]%10);
	FOR(i,0,3) movecnt2+=calc_Move(chess[2][i]/10,chess[2][i]%10);
	m=1.0*movecnt1/(movecnt2+0.00001);
	db ret;
	if(turn<=10) ret=k1[0]*tq+k2[0]*tk+k3[0]*p1+k4[0]*p2+k5[0]*m;
	else if(turn<=25) ret=k1[1]*tq+k2[1]*tk+k3[1]*p1+k4[1]*p2+k5[1]*m;
	else ret=k1[2]*tq+k2[2]*tk+k3[2]*p1+k4[2]*p2+k5[2]*m;
	return ret;
}

db Board::eval() {
	Board* node=new Board;
	node->copy(*this);
	int SIM=0;
	if(player==uct_player) {
		if(turn<=10) SIM=2;
		else SIM=4;
	}
	else {
		if(turn<=10) SIM=1;
		else SIM=3;
	}
	FOR(i,1,SIM) {
		// cerr<<"Simulate "<<i<<"\n";
		int Sol=randSol(nullptr,node->player);
		if(Sol<0) break;
		// cerr<<"randSol "<<Sol<<"\n";
		node->Move(Sol/100000,Sol/10000%10,Sol/1000%10,Sol/100%10,Sol/10%10,Sol%10);
		node->player=3-node->player;
	}
	// cerr<<"pure_eval start"<<"\n";
	db ret=node->pure_eval();
	// cerr<<"pure_eval end "<<ret<<"\n";
	if(uct_player==BLACK) return ret;
	else return -ret;
}

Board* Board::expand() {
	int Sol=randSol(this,player);
	// cerr<<"randSol "<<Sol<<"\n";
	if(Sol<0) return nullptr;
	// cerr<<"randSol "<<Sol<<"\n";
	Board* node = new Board;
	node->copy(*this);
	node->Move(Sol/100000,Sol/10000%10,Sol/1000%10,Sol/100%10,Sol/10%10,Sol%10);
	node->fat_move=Sol;
	// node->Show();
	node->score=node->eval();
	// node->Show();
	Son.pb(node);
	Used.insert(Sol);
	return node;
}

int Search_cnt;

void Board::Search() {
	Board* cur=this;
	vector<Board*> chain; 
	chain.pb(cur);
	// cur->Show();
	while(cur->noSon()&&!cur->isLeaf()) {
		cur=cur->Bestfornow();
		chain.pb(cur);
	}
	// cur->Show();
	Board* Son=cur->expand();
	// Son->Show();
	if(Son!=nullptr) {
		for(Board* &it:chain)
			it->visits++,it->score+=Son->score;
	}
	++Search_cnt;
}