#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
typedef struct Card//卡牌定义
{
	int Level;//卡牌等级
	int Type;//卡牌类型:攻击\防御
	//攻击牌:type=1 防御牌:type=0
	int Kind;//卡牌种类:体术等
	//体术kind=0 剑kind=1 弓kind=2 刀kind=3 刺kind=4 枪kind=5
	int Fun[15];//卡牌效果
	//压制fun=0 制衡fun=1 不屈fun=2 攻心fun=3 追击fun=4
	//标记fun=5 中毒fun=6 封印fun=7 易伤fun=7 穿刺fun=8
	char Name[10];//卡牌名称
	char text[100];//卡牌描述
}Card;
typedef struct Cards//牌库定义
{
	Card cards[100];
	int NUM;
}Cards;
typedef struct Deck//套牌定义
{
	Card card[20];
	int CardNum = 0;
}Deck;
typedef struct Hand//手牌定义
{
	Card card[5];
	int CardNum = 0;
	int AttackNum=0;
	int DefenceNum=0;
}Hand;
typedef struct player//玩家定义
{
	int bot=0;
	char playername[10];
	int HP = 10;//玩家生命
	Hand hand;//玩家手牌
	Deck deck;//玩家套牌
	Deck deck_inturn;//临时套牌
	int Attack_level_plus = 0;//攻击等级+1
	int Defence_level_plus = 0;//防御等级+1
	int Card_level_plus = 0;//卡牌等级+1
	int Poisoned = 0;//中毒
	int Marked[5];//标记
	int Sealed[5];//封印
	int Turn_mode;//回合模式,1为进攻,0为防守
    Card Card_used;//上回合使用卡牌
    int action;//上回合行动,1为出牌,0为抽牌
}player;
/*------------------------------定义部分--------------------------------------------*/
int Name_Initialize(player& player1, player& player2);
int Cards_Initialize(Cards& cards);
void Deck_Initialize(player &player);
int PlayTurn(Cards cards, player& player1, player& player2);
int StartTurn(player& player1, player& player2);
int Start_Drawing(player &player);
int Drawing(player &player);
int First_Attack_Decide();
int Choose(player& player);
int Use(player&player);
void Show_Turn(player player);
void Show_HP(player player);
void Show_Hand(player player);
void Show_Card(Card card);
void Show_Deck(Deck deck);
int Settlement(player& player1,player& player2);
void Level_calculate(player &player);
void Card_damage(player& player1,player& player2);
int If_Over(player player1,player player2);
int PlayTurn_Over(player player1, player player2, int flag);
int GetCard(Cards &cards,player &player1,player &player2,int winner);
void Card_function(player &player1,player &player2);
int Fun_0_yazhi(player &player1,player &player2);
int Bot_choose(player &player);
int Bot_Draw(player &player);
int Bot_Use(player &player);
/*------------------------------卡牌效果函数----------------------------------------*/
void Ge_Dang(player &player1,player &player2);
/*------------------------------函数部分--------------------------------------------*/
int main()
{
	Cards Cards;
	player player1;
	player player2;
	int op,winner;
	if (Cards_Initialize(Cards))
	{
		printf("   卡牌读取完毕!\n   按下回车开始!\n");
		getchar();
		system("cls");
	}
	else
	{
		printf("   卡牌读取失败!\n");
		getchar();
		return 0;
	}
	{//选择是否与电脑进行游戏
		printf("  是否与电脑对战?\n 1.是 0.否\n");
		scanf("%d",&op);
		if(op)
		player2.bot=1;
		else;
		system("cls");
	}
	Deck_Initialize(player1);
	Deck_Initialize(player2);
	while(1)
	{
		winner=PlayTurn(Cards, player1, player2);
		GetCard(Cards,player1,player2,winner);
	}
	return 0;
}
int Name_Initialize(player& player1, player& player2)
{
	strcpy(player1.playername,"玩家1");
	if(!player2.bot)
	strcpy(player2.playername,"玩家2");
	else
	strcpy(player2.playername,"弱智电脑");
}
void Deck_Initialize(player &player)//卡组初始化
{
	srand(time(NULL));
	for (int i = 0; i < 5; i++)
	{
		strcpy(player.deck.card[i].Name, "拳");
		strcpy(player.deck.card[i].text, "无");
		player.deck.card[i].Level = 1;
		player.deck.card[i].Kind = 0;
		player.deck.card[i].Type = 1;
	}
	for (int i = 5; i < 7; i++)
	{
		strcpy(player.deck.card[i].Name, "腿");
		strcpy(player.deck.card[i].text, "无");
		player.deck.card[i].Level = 2;
		player.deck.card[i].Kind = 0;
		player.deck.card[i].Type = 1;
	}
	for (int i = 7; i < 10; i++)
	{
		strcpy(player.deck.card[i].Name, "格挡");
		strcpy(player.deck.card[i].text, "压制:抽一张牌或打出一张攻击牌");
		player.deck.card[i].Level = 2;
		player.deck.card[i].Fun[0] = 1;
		player.deck.card[i].Kind = 0;
		player.deck.card[i].Type = 0;
	}
	player.deck.CardNum = 10;
}
int Cards_Initialize(Cards& Cards)//从文件中获取卡牌
{
	FILE* fp;
	int fun;
	if (fp = fopen(".\\card.txt", "r"));
	else return 0;
	fscanf(fp,"%d",&Cards.NUM);
	for (int i = 0; i < Cards.NUM; i++)
	{
		fscanf(fp, "%s %d %d %d %s", Cards.cards[i].Name,&Cards.cards[i].Type, &Cards.cards[i].Level, &Cards.cards[i].Kind, Cards.cards[i].text);
		do
		{
			fscanf(fp,"%d",&fun);
			Cards.cards[i].Fun[fun] = 1;
		} while (fun);
		Cards.cards[i+Cards.NUM]=Cards.cards[i];
		Cards.cards[i+2*Cards.NUM]=Cards.cards[i];
	}
	Cards.NUM*=3;
	fclose(fp);
	return 1;
}
int PlayTurn(Cards cards, player& player1, player& player2)//游戏回合
{
	int TurnNum = 1,Over_flag=0;
	StartTurn(player1, player2);
	while (1)
	{
        if(player1.Turn_mode)
        {
			printf(" 第%d回合:\n",TurnNum);
            player1.action=Choose(player1);
			printf(" 第%d回合:\n",TurnNum);
			if(!player2.bot)
            player2.action=Choose(player2);
			else
			player2.action=Bot_choose(player2);
        }
        else
        {
			printf(" 第%d回合:\n",TurnNum);
            if(!player2.bot)
            player2.action=Choose(player2);
			else
			player2.action=Bot_choose(player2);
			printf(" 第%d回合:\n",TurnNum);
            player1.action=Choose(player1);
        }
		if(player1.action==2&&player2.action!=2)
		{
			Over_flag=2;
			break;
		}
		else if(player2.action==2&&player1.action!=2)
		{
			Over_flag=1;
			break;
		}
		else if(player1.action==2&&player2.action==2)
		{
			Over_flag=3;
			break;
		}
		else
		Settlement(player1,player2);
        Over_flag=If_Over(player1,player2);
		if(Over_flag!=0)
		break;
		TurnNum++;
	}
	return (PlayTurn_Over(player1,player2,Over_flag));
}
int StartTurn(player& player1, player& player2)//游戏开始前抽牌和决定攻守方
{	
	Name_Initialize(player1,player2);
	player1.deck_inturn = player1.deck;//回合中使用的套牌复制
	player2.deck_inturn = player2.deck;
	Start_Drawing(player1);
	Start_Drawing(player2);
	if (First_Attack_Decide())
		player1.Turn_mode = 1;
	else
		player2.Turn_mode = 1;
    if(player1.Turn_mode)
    printf(" %s 获得先攻!\n",player1.playername);
    else
    printf(" %s 获得先攻!\n",player2.playername);
    getchar();
	return player1.Turn_mode;
}
int First_Attack_Decide()/*决定先攻*/
{
	return rand() % 2;
}
int Start_Drawing(player &player)/*初始抽牌*/
{
	player.hand.CardNum=0;
	player.hand.AttackNum=0;
	player.hand.DefenceNum=0;
	for (int i = 0; i < 5; i++)
	{
		int draw_loc;
		draw_loc = rand() % player.deck_inturn.CardNum;
		player.hand.card[player.hand.CardNum] = player.deck_inturn.card[draw_loc];
		for (int j = draw_loc; j + 1 < player.deck_inturn.CardNum; j++)
		player.deck_inturn.card[j] = player.deck_inturn.card[j + 1];
		if(player.hand.card[player.hand.CardNum].Type)
		player.hand.AttackNum++;
		else
		player.hand.DefenceNum++;
		player.deck_inturn.CardNum--;
		player.hand.CardNum++;
		
	}
	return 0;
}
int Drawing(player &player)/*抽牌阶段*/
{
	Card drawd;
	int draw_loc;
	draw_loc = rand() % player.deck_inturn.CardNum;
	drawd = player.deck_inturn.card[draw_loc];
	for (int j = draw_loc; j + 1 < player.deck_inturn.CardNum; j++)
		player.deck_inturn.card[j] = player.deck_inturn.card[j + 1];
	player.deck_inturn.CardNum--;
	if (player.hand.CardNum == 5)
	{
		printf(" 手牌已满,卡牌 %s 进入弃牌库!\n", drawd.Name);
		getchar();
		getchar();
		system("cls");
		return -1;
	}
	else
		player.hand.card[player.hand.CardNum] = drawd;
	printf(" 抽到了卡牌 %s !\n", drawd.Name);
	if(drawd.Type)
	player.hand.AttackNum++;
	else
	player.hand.DefenceNum++;
	getchar();
	getchar();
	player.hand.CardNum++;
	return 1;
}
int Choose(player &player)/*选牌阶段*/
{
	int op;
	while(1)
	{
		Show_Turn(player);
		Show_HP(player);
		Show_Hand(player);
		if(player.hand.CardNum!=0)//可以出牌
		{
			if(player.deck_inturn.CardNum==0)//无法抽牌
			{
				printf(" 请选择出牌,跳过回合或认输\n");
				printf(" 1.出牌 2.跳过回合 0.认输\n");
			}
			else//可以抽牌
			{
				printf(" 请选择出牌,抽牌或认输\n");
				printf(" 1.出牌 2.抽牌 0.认输\n");
			}
			scanf("%d", &op);
		}
		else//无法出牌
		{
			op=2;
			if(player.deck_inturn.CardNum==0)//无法抽牌
			printf(" 无法出牌,自动跳过回合\n");
			else//可以抽牌
			printf(" 无法出牌,自动抽牌\n");
		}
		if(op==1)
		{
			int Use_flag=Use(player);
			if(Use_flag==1)//成功使用卡牌
			{
				system("cls");
				return 1;//使用卡牌
			}
			else if(Use_flag==2)//无法使用卡牌
			op=2;
			else//取消出牌
			continue;//重新开始回合
		}
		if(op==2)
		{
			if(player.deck_inturn.CardNum!=0)//抽牌
			{
				if(Drawing(player))
				{
					system("cls");
					return 0;//抽牌
				}
				else
				{
					system("cls");
					continue;//重新开始回合
				}
			}
			else//跳过
			{
				printf(" 已跳过该回合!\n");
				getchar();
				system("cls");
				return -1;//跳过回合
			}
		}
		else
		{
			printf(" 已认输!\n");
			getchar();
			getchar();
			system("cls");
			return 2;//认输
		}
	}
}
int Use(player&player)/*用牌阶段*/
{
	int Card_loc;
	if(player.Turn_mode)//进攻回合
	{
		if(player.hand.AttackNum!=0)//手牌中有攻击牌
		{
			printf(" 请选择攻击牌打出! 1~%d\n", player.hand.CardNum);
			printf(" 取消选择请输入 0\n");
		    scanf("%d",&Card_loc);
			if (!Card_loc)
			{
				getchar();
				getchar();
				system("cls");
				return 0;//放弃使用卡牌
			}
			else
			{
				player.Card_used=player.hand.card[Card_loc-1];
				if((!player.Card_used.Type)&&(player.Turn_mode))
                {
                    printf(" 选择的非攻击牌,请重新选择!");
                    getchar();
                    getchar();
                    system("cls");
                    return 0;//选择卡牌错误
                }
				for (int i = Card_loc - 1; i + 1 < player.hand.CardNum; i++)
				player.hand.card[i] = player.hand.card[i + 1];
				player.hand.CardNum--;
				printf(" 卡牌 %s 将会打出!\n", player.Card_used.Name);
				player.hand.AttackNum--;
				getchar();
				getchar();
				system("cls");
				return 1;//成功使用卡牌
			}
		}
		else//手牌中没有攻击牌
		{
			if(player.deck_inturn.CardNum!=0)//可以抽牌
			{
				printf(" 手牌中没有攻击牌,自动抽牌!\n");
				return 2;//抽牌
			}
			else
			{
				printf(" 手牌中没有攻击牌,无法抽牌,自动跳过回合!\n");
				return 2;//跳过
			}
		}
	}
	else//防御回合
	{
		printf(" 请任意选择一张牌打出! 1~%d \n",player.hand.CardNum);
		printf(" 取消选择请输入 0\n");
		scanf("%d",&Card_loc);
		if (!Card_loc)
		{
			getchar();
			system("cls");
			return 0;//放弃使用卡牌
		}
		else
		{
			player.Card_used=player.hand.card[Card_loc-1];
			for (int i = Card_loc - 1; i + 1 < player.hand.CardNum; i++)
			player.hand.card[i] = player.hand.card[i + 1];
			player.hand.CardNum--;
			printf(" 卡牌 %s 将会打出!\n", player.Card_used.Name);
			if(player.Card_used.Type)//打出攻击牌
			player.hand.AttackNum--;
			else
			player.hand.DefenceNum--;
			getchar();
			system("cls");
			return 1;//成功使用卡牌
		}
	}
}
void Show_Turn(player player)//显示回合模式
{
	printf("\n当前为 %s 的",player.playername);
	if(player.Turn_mode)
	printf(" 进攻回合\n");
	else
	printf(" 防御回合\n");
}
void Show_HP(player player)//显示血量
{
	printf(" 当前血量: ");
	for (int i = 0; i < 10; i++)
	{
		if (i < player.HP)
			printf("■");
		else
			printf("□");
	}
	printf(" 共%d点\n", player.HP);
}
void Show_Hand(player player)//显示手牌
{
	printf(" 当前手牌: ");
	printf("         牌库剩余卡牌: %d \n",player.deck_inturn.CardNum);
	if(player.hand.CardNum==0)
	{
		printf(" 无 \n");
		return;
	}
	for (int i = 0; i < player.hand.CardNum; i++)
		printf(" %d.%s ", i + 1, player.hand.card[i].Name);
	printf("\n");
}
void Show_Card(Card card)//显示卡牌信息
{
	if(card.Kind==0)
	printf(" 体术 ");
	printf(" 卡牌 %s \n",card.Name);
	if(card.Type)
	printf(" 攻击等级:");
	else
	printf(" 防御等级:");
	printf(" %d\n",card.Level);
	printf(" %s\n",card.text);
}
void Show_Deck(Deck deck)//显示套牌信息
{
	for(int i=0;i<deck.CardNum;i++)
	{
		printf("%d.%-8s",i+1,deck.card[i].Name);
		if(!(i%5))
		printf("\n");
	}
}
int Settlement(player& player1,player& player2)/*结算阶段*/
{
    Level_calculate(player1);
    Level_calculate(player2);
	Card_damage(player1,player2);
	Card_function(player1,player2);
    return 0;
}
void Level_calculate(player &player)//计算卡牌等级
{
    if(player.Card_level_plus)
	{ 
		player.Card_used.Level++;
		player.Card_level_plus=0;
	}
    if(player.Attack_level_plus&&player.Card_used.Type)
    {
		player.Card_used.Level++;
		player.Attack_level_plus=0;
	}
    if(player.Defence_level_plus&&(!player.Card_used.Type))
    {
		player.Card_used.Level++;
		player.Defence_level_plus=0;
	}
}
void Card_damage(player& player1,player& player2)//计算卡牌等级造成的伤害
{
	int damage;
	if(player1.Turn_mode)
    {
        printf(" 进攻方 %s 选择 ",player1.playername);
        if(player1.action==1)
        printf("出牌\n %s 打出的牌是:%2s  卡牌等级:%d  卡牌效果:%s\n\n",player1.playername,player1.Card_used.Name,player1.Card_used.Level,player1.Card_used.text);
        else if(player1.action==0)
        printf("抽牌\n\n");
		else 
		printf("跳过回合\n");
        printf(" 防御方 %s 选择 ",player2.playername);
        if(player2.action==1)
        printf("出牌\n %s 打出的牌是:%2s  卡牌等级:%d  卡牌效果:%s\n\n",player2.playername,player2.Card_used.Name,player2.Card_used.Level,player2.Card_used.text);
        else if(player2.action==0)
        printf("抽牌\n\n");
		else 
		printf("跳过回合\n");
        printf(" 结算结果:");
        if(player1.action==1&&player2.action==1&&(player1.Card_used.Level>player2.Card_used.Level))
        {
            damage=player1.Card_used.Level-player2.Card_used.Level;
            player2.HP-=damage;
            printf(" %s 对 %s 造成了 %d 点伤害!\n",player1.playername,player2.playername,damage);
        }
		else if(player1.action==1&&player2.action!=1)
		{
			damage=player1.Card_used.Level;
			player2.HP-=damage;
			printf(" %s 对 %s 造成了 %d 点伤害!\n",player1.playername,player2.playername,damage);
		}
        else if(player1.action!=1||player1.Card_used.Level<player2.Card_used.Level)
        {
            player1.Turn_mode=0;
            player2.Turn_mode=1;
            printf(" 攻守交换!");
        }
		else printf(" 无事发生");
		getchar();
		system("cls");
    }
    else
    {
        printf(" 进攻方 %s 选择 ",player2.playername);
        if(player2.action==1)
        printf("出牌\n %s 打出的牌是 %4s  卡牌等级 %d  卡牌效果 %s\n",player2.playername,player2.Card_used.Name,player2.Card_used.Level,player2.Card_used.text);
        else if(player2.action==0)
        printf("抽牌\n\n");
		else 
		printf("跳过回合\n");
        printf(" 防御方 %s 选择 ",player1.playername);
        if(player1.action==1)
        printf("出牌\n %s 打出的牌是 %4s  卡牌等级 %d  卡牌效果 %s\n",player1.playername,player1.Card_used.Name,player1.Card_used.Level,player1.Card_used.text);
        else if(player1.action==0)
        printf("抽牌\n\n");
		else 
		printf("跳过回合\n");
		printf(" 结算结果:");
        if(player1.action==1&&player2.action==1&&(player2.Card_used.Level>player1.Card_used.Level))
        {
            damage=player2.Card_used.Level-player1.Card_used.Level;
            player1.HP-=damage;
            printf(" %s 对 %s 造成了 %d 点伤害!\n",player2.playername,player1.playername,damage);
        }
		else if(player2.action==1&&player1.action!=1)
		{
			damage=player2.Card_used.Level;
			player1.HP-=damage;
			printf(" %s 对 %s 造成了 %d 点伤害!\n",player2.playername,player1.playername,damage);
		}
		else if(player2.action!=1||player2.Card_used.Level<player1.Card_used.Level)
        {
            player2.Turn_mode=0;
            player1.Turn_mode=1;
            printf(" 攻守交换!");
        }
		else printf(" 无事发生");
		getchar();
		system("cls");
    }
}
int If_Over(player player1,player player2)//判断游戏是否结束.
{
	if(player1.action==2&&player2.action!=2)
	return 2;//玩家2胜利
	else if(player2.action==2&&player1.action!=2)
	return 1;//玩家1胜利
	else if(player1.action==2&&player2.action==2)
	return 3;//平局
	if((player1.hand.CardNum==0)&&(player2.hand.CardNum==0)&&(player2.deck_inturn.CardNum==0)&&(player1.deck_inturn.CardNum==0))
	{
		if(player1.HP>player2.HP)
		return 1;//玩家1胜利
		else if(player2.HP>player1.HP)
		return 2;//玩家2胜利
		else
		return 3;//平局
	}
	return 0;//游戏未结束
}
int PlayTurn_Over(player player1, player player2, int flag)//游戏回合结束
{
	if(flag==1)
	{
		if(player2.action==2)
		printf(" \n%s 认输, %s 获胜!\n",player2.playername,player1.playername);
		else
		{
			printf(" \n%s 剩余体力: %d  \n%s 剩余体力: %d \n",player2.playername,player1.HP,player1.playername,player2.HP);
			printf(" \n%s 被击败, %s 获胜\n",player2.playername,player1.playername);
		}
		getchar();
		system("cls");
		return 1;//玩家1赢
	}
	else if(flag==2)
	{
		if(player1.action==2)
		printf(" \n%s 认输, %s 获胜!\n",player1.playername,player2.playername);
		else
		{
			printf(" \n%s 剩余体力: %d  \n%s 剩余体力: %d \n",player1.playername,player1.HP,player1.playername,player2.HP);
			printf(" \n%s 被击败, %s 获胜\n",player1.playername,player2.playername);
		}
		getchar();
		system("cls");
		return 2;//玩家2赢
	}
	else
	{
		if(player1.action==2)
		printf(" \n双方同时认输,平局!\n");
		else
		printf(" \n双方剩余体力相等,平局!\n");
		getchar();
		system("cls");
		return 0;//平局
	}
}
int GetCard(Cards &Cards,player &player1,player &player2,int winner)//卡牌获取函数
{
	Card card1,card2;
	int Card_loc1,Card_loc2,op;
	Card_loc1=rand()%Cards.NUM;
	card1=Cards.cards[Card_loc1];
	Card_loc2=rand()%Cards.NUM;
	card2=Cards.cards[Card_loc2];
	if(winner==0)//平局
	{
		{//玩家1获得卡牌
			printf(" %s 抽到了卡牌 %s!\n\n",player1.playername,card1.Name);
			Show_Card(card1);
			if(player1.deck.CardNum<20)//套牌未满
			{
				printf("已将 %s 加入套牌中!\n",card1.Name);
				player1.deck.card[player1.deck.CardNum]=card1;//获得卡牌
				Cards.NUM--;
				player1.deck.CardNum++;
				for(int i=Card_loc1;i<Cards.NUM;i++)
				Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
			}
			else//套牌已满
			{
				printf(" 套牌已满,请选择一张牌与该牌交换或放弃获得该牌!\n");
				Show_Deck(player1.deck);
				printf(" 1~20.交换 0.放弃\n");
				scanf("%d",&op);
				if(op==0)
				printf(" 放弃获得卡牌 %s !\n",card1.Name);
				else
				{
					if((strcmp(player1.deck.card[op-1].Name,"拳")!=0)&&(strcmp(player1.deck.card[op-1].Name,"腿")!=0)&&(strcmp(player1.deck.card[op-1].Name,"格挡")!=0))//卡牌非初始卡牌
					Cards.cards[Card_loc1]=player1.deck.card[op-1];//交换牌库中卡牌
					else;//卡牌是初始卡牌
					player1.deck.card[op-1]=card1;//获得卡牌
					Cards.NUM--;
					for(int i=Card_loc1;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
				}
			}
			getchar();
			system("cls");
		}
		{//玩家2获得卡牌
			printf(" %s 抽到了卡牌 %s!\n\n",player2.playername,card2.Name);
			Show_Card(card2);
			if(player2.deck.CardNum<20)//套牌未满
			{
				printf("已将 %s 加入套牌中!\n",card2.Name);
				player2.deck.card[player2.deck.CardNum]=card2;//获得卡牌
				Cards.NUM--;
				player2.deck.CardNum++;
				for(int i=Card_loc2;i<Cards.NUM;i++)
				Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
			}
			else//套牌已满
			{
				printf(" 套牌已满,请选择一张牌与该牌交换或放弃获得该牌!\n");
				Show_Deck(player2.deck);
				printf(" 1~20.交换 0.放弃\n");
				scanf("%d",&op);
				if(op==0)
				printf(" 放弃获得卡牌 %s !\n",card2.Name);
				else
				{
					if((strcmp(player2.deck.card[op-1].Name,"拳")!=0)&&(strcmp(player2.deck.card[op-1].Name,"腿")!=0)&&(strcmp(player2.deck.card[op-1].Name,"格挡")!=0))//卡牌非初始卡牌
					Cards.cards[Card_loc2]=player2.deck.card[op-1];//交换牌库中卡牌
					else;//卡牌是初始卡牌
					player2.deck.card[op-1]=card2;//获得卡牌
					Cards.NUM--;
					for(int i=Card_loc2;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
				}
			}
			getchar();
			system("cls");
		}
	}
	else if(winner==1)//玩家1胜利
	{
		printf(" 抽到了以下卡牌:\n");
		printf(" 卡牌1: %s ",card1.Name);
		Show_Card(card1);
		printf(" 卡牌2: %s ",card2.Name);
		Show_Card(card2);
		printf(" 请 %s 选择一张牌加入套牌中,或者放弃选择!\n",player1.playername);
		printf(" 1.卡牌1 2.卡牌2 0.放弃选择\n");
		scanf("%d",&op);
		if(op==0);//放弃
		else if(op==1)//卡牌1
		{
			{//玩家1获得卡牌
				printf(" %s ",player1.playername);
				if(player1.deck.CardNum<20)//套牌未满
				{
					printf("已将 %s 加入的套牌中!\n",card1.Name);
					player1.deck.card[player1.deck.CardNum]=card1;//获得卡牌
					Cards.NUM--;
					player1.deck.CardNum++;
					for(int i=Card_loc1;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
				}
				else//套牌已满
				{
					printf(" 套牌已满,请选择一张牌与该牌交换或放弃获得该牌!\n");
					Show_Deck(player1.deck);
					printf(" 1~20.交换 0.放弃\n");
					scanf("%d",&op);
					if(op==0)
					printf(" 放弃获得卡牌 %s !\n",card1.Name);
					else
					{
						if((strcmp(player1.deck.card[op-1].Name,"拳")!=0)&&(strcmp(player1.deck.card[op-1].Name,"腿")!=0)&&(strcmp(player1.deck.card[op-1].Name,"格挡")!=0))//卡牌非初始卡牌
						Cards.cards[Card_loc1]=player1.deck.card[op-1];//交换牌库中卡牌
						else;//卡牌是初始卡牌
						player1.deck.card[op-1]=card1;//获得卡牌
						Cards.NUM--;
						player2.deck.CardNum++;
						for(int i=Card_loc1;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
					}
				}
				getchar();
				getchar();
				system("cls");
			}
			{//玩家2获得卡牌
				printf(" %s ",player2.playername);
				if(player2.deck.CardNum<20)//套牌未满
				{
					printf("已将 %s 加入套牌中!\n",card2.Name);
					player2.deck.card[player2.deck.CardNum]=card2;//获得卡牌
					Cards.NUM--;
					player2.deck.CardNum++;
					for(int i=Card_loc2;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
				}
				else//套牌已满
				{
					printf(" 套牌已满,请选择一张牌与该牌交换或放弃获得该牌!\n");
					Show_Deck(player2.deck);
					printf(" 1~20.交换 0.放弃\n");
					scanf("%d",&op);
					if(op==0)
					printf(" 放弃获得卡牌 %s !\n",card2.Name);
					else
					{
						if((strcmp(player2.deck.card[op-1].Name,"拳")!=0)&&(strcmp(player2.deck.card[op-1].Name,"腿")!=0)&&(strcmp(player2.deck.card[op-1].Name,"格挡")!=0))//卡牌非初始卡牌
						Cards.cards[Card_loc2]=player2.deck.card[op-1];//交换牌库中卡牌
						else;//卡牌是初始卡牌
						player2.deck.card[op-1]=card2;//获得卡牌
						Cards.NUM--;
						for(int i=Card_loc2;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
					}
				}
				getchar();
				system("cls");
			}
		}
		else//卡牌2
		{
			{//玩家1获得卡牌
				printf(" %s ",player1.playername);
				if(player1.deck.CardNum<20)//套牌未满
				{
					printf("已将 %s 加入套牌中!\n",card2.Name);
					player1.deck.card[player1.deck.CardNum]=card2;//获得卡牌
					Cards.NUM--;
					player1.deck.CardNum++;
					for(int i=Card_loc2;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
				}
				else//套牌已满
				{
					printf(" 套牌已满,请选择一张牌与该牌交换或放弃获得该牌!\n");
					Show_Deck(player1.deck);
					printf(" 1~20.交换 0.放弃\n");
					scanf("%d",&op);
					if(op==0)
					printf(" 放弃获得卡牌 %s !\n",card2.Name);
					else
					{
						if((strcmp(player1.deck.card[op-1].Name,"拳")!=0)&&(strcmp(player1.deck.card[op-1].Name,"腿")!=0)&&(strcmp(player1.deck.card[op-1].Name,"格挡")!=0))//卡牌非初始卡牌
						Cards.cards[Card_loc2]=player1.deck.card[op-1];//交换牌库中卡牌
						else;//卡牌是初始卡牌
						player1.deck.card[op-1]=card2;//获得卡牌
						Cards.NUM--;
						for(int i=Card_loc2;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
					}
				}
				getchar();
				getchar();
				system("cls");
			}
			{//玩家2获得卡牌
				printf(" %s ",player2.playername);
				if(player2.deck.CardNum<20)//套牌未满
				{
					printf("已将 %s 加入套牌中!\n",card1.Name);
					player2.deck.card[player2.deck.CardNum]=card1;//获得卡牌
					Cards.NUM--;
					player2.deck.CardNum++;
					for(int i=Card_loc1;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
				}
				else//套牌已满
				{
					printf(" 套牌已满,请选择一张牌与该牌交换或放弃获得该牌!\n");
					Show_Deck(player2.deck);
					printf(" 1~20.交换 0.放弃\n");
					scanf("%d",&op);
					if(op==0)
					printf(" 放弃获得卡牌 %s !\n",card1.Name);
					else
					{
						if((strcmp(player2.deck.card[op-1].Name,"拳")!=0)&&(strcmp(player2.deck.card[op-1].Name,"腿")!=0)&&(strcmp(player2.deck.card[op-1].Name,"格挡")!=0))//卡牌非初始卡牌
						Cards.cards[Card_loc1]=player2.deck.card[op-1];//交换牌库中卡牌
						else;//卡牌是初始卡牌
						player2.deck.card[op-1]=card1;//获得卡牌
						Cards.NUM--;
						for(int i=Card_loc1;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
					}
				}
				getchar();
				getchar();
				system("cls");
			}
		}
	}
	else//玩家2胜利
	{
		printf(" 抽到了以下卡牌:\n");
		printf(" 卡牌1: %s ",card1.Name);
		Show_Card(card1);
		printf(" 卡牌2: %s ",card2.Name);
		Show_Card(card2);
		printf(" 请选择一张牌加入套牌中,或者放弃选择!\n");
		printf(" 1.卡牌1 2.卡牌2 0.放弃选择\n");
		scanf("%d",&op);
		if(op==0);//放弃
		else if(op==1)//卡牌1
		{
			{//玩家2获得卡牌
				printf(" %s ",player2.playername);
				if(player2.deck.CardNum<20)//套牌未满
				{
					printf("已将 %s 加入套牌中!\n",card1.Name);
					player2.deck.card[player2.deck.CardNum]=card1;//获得卡牌
					Cards.NUM--;
					player2.deck.CardNum++;
					for(int i=Card_loc1;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
				}
				else//套牌已满
				{
					printf(" 套牌已满,请选择一张牌与该牌交换或放弃获得该牌!\n");
					Show_Deck(player2.deck);
					printf(" 1~20.交换 0.放弃\n");
					scanf("%d",&op);
					if(op==0)
					printf(" 放弃获得卡牌 %s !\n",card1.Name);
					else
					{
						if((strcmp(player2.deck.card[op-1].Name,"拳")!=0)&&(strcmp(player2.deck.card[op-1].Name,"腿")!=0)&&(strcmp(player2.deck.card[op-1].Name,"格挡")!=0))//卡牌非初始卡牌
						Cards.cards[Card_loc1]=player2.deck.card[op-1];//交换牌库中卡牌
						else;//卡牌是初始卡牌
						player2.deck.card[op-1]=card1;//获得卡牌
						Cards.NUM--;
						for(int i=Card_loc1;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
					}
				}
				getchar();
				getchar();
				system("cls");
			}
			{//玩家1获得卡牌
				printf(" %s ",player1.playername);
				if(player1.deck.CardNum<20)//套牌未满
				{
					printf("已将 %s 加入套牌中!\n",card2.Name);
					player1.deck.card[player1.deck.CardNum]=card2;//获得卡牌
					Cards.NUM--;
					player1.deck.CardNum++;
					for(int i=Card_loc2;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
				}
				else//套牌已满
				{
					printf(" 套牌已满,请选择一张牌与该牌交换或放弃获得该牌!\n");
					Show_Deck(player1.deck);
					printf(" 1~20.交换 0.放弃\n");
					scanf("%d",&op);
					if(op==0)
					printf(" 放弃获得卡牌 %s !\n",card2.Name);
					else
					{
						if((strcmp(player1.deck.card[op-1].Name,"拳")!=0)&&(strcmp(player1.deck.card[op-1].Name,"腿")!=0)&&(strcmp(player1.deck.card[op-1].Name,"格挡")!=0))//卡牌非初始卡牌
						Cards.cards[Card_loc2]=player1.deck.card[op-1];//交换牌库中卡牌
						else;//卡牌是初始卡牌
						player1.deck.card[op-1]=card2;//获得卡牌
						Cards.NUM--;
						for(int i=Card_loc2;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
					}
				}
				getchar();
				system("cls");
			}
		}
		else//卡牌2
		{
			{//玩家2获得卡牌
				printf(" %s ",player2.playername);
				if(player2.deck.CardNum<20)//套牌未满
				{
					printf("已将 %s 加入套牌中!\n",card2.Name);
					player2.deck.card[player2.deck.CardNum]=card2;//获得卡牌
					Cards.NUM--;
					player2.deck.CardNum++;
					for(int i=Card_loc2;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
				}
				else//套牌已满
				{
					printf(" 套牌已满,请选择一张牌与该牌交换或放弃获得该牌!\n");
					Show_Deck(player2.deck);
					printf(" 1~20.交换 0.放弃\n");
					scanf("%d",&op);
					if(op==0)
					printf(" 放弃获得卡牌 %s !\n",card2.Name);
					else
					{
						if((strcmp(player2.deck.card[op-1].Name,"拳")!=0)&&(strcmp(player2.deck.card[op-1].Name,"腿")!=0)&&(strcmp(player2.deck.card[op-1].Name,"格挡")!=0))//卡牌非初始卡牌
						Cards.cards[Card_loc2]=player2.deck.card[op-1];//交换牌库中卡牌
						else;//卡牌是初始卡牌
						player2.deck.card[op-1]=card2;//获得卡牌
						Cards.NUM--;
						for(int i=Card_loc2;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
					}
				}
				getchar();
				getchar();
				system("cls");
			}
			{//玩家1获得卡牌
			printf(" %s ",player1.playername);
				if(player1.deck.CardNum<20)//套牌未满
				{
					printf("已将 %s 加入套牌中!\n",card1.Name);
					player1.deck.card[player1.deck.CardNum]=card1;//获得卡牌
					Cards.NUM--;
					player1.deck.CardNum++;
					for(int i=Card_loc1;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
				}
				else//套牌已满
				{
					printf(" 套牌已满,请选择一张牌与该牌交换或放弃获得该牌!\n");
					Show_Deck(player1.deck);
					printf(" 1~20.交换 0.放弃\n");
					scanf("%d",&op);
					if(op==0)
					printf(" 放弃获得卡牌 %s !\n",card1.Name);
					else
					{
						if((strcmp(player1.deck.card[op-1].Name,"拳")!=0)&&(strcmp(player1.deck.card[op-1].Name,"腿")!=0)&&(strcmp(player1.deck.card[op-1].Name,"格挡")!=0))//卡牌非初始卡牌
						Cards.cards[Card_loc1]=player1.deck.card[op-1];//交换牌库中卡牌
						else;//卡牌是初始卡牌
						player1.deck.card[op-1]=card1;//获得卡牌
						Cards.NUM--;
						for(int i=Card_loc1;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//从牌库中移除卡牌
					}
				}
				getchar();
				getchar();
				system("cls");
			}
		}
	}
	return 0;
}
void Card_function(player &player1,player &player2)//卡牌效果触发函数
{
	if(player1.action==1&&player2.action==1)//双方都出牌
	{
		//玩家1使用的卡牌效果判断--------------------------------------------
		if(player1.Card_used.Fun[1])//压制效果
		{
			if(player1.Card_used.Level>player2.Card_used.Level)
			Fun_0_yazhi(player1,player2);
		}

		//玩家2使用的卡牌效果判断--------------------------------------------
		if(player2.Card_used.Fun[1])//压制效果
		{
			if(player2.Card_used.Level>player1.Card_used.Level)
			Fun_0_yazhi(player2,player1);
		}
		return ;
	}
	else//攻心
	;
}
int Fun_0_yazhi(player &player1,player &player2)//所有压制效果触发函数
{
	if(!strcmp(player1.Card_used.Name,"格挡"))// 格挡 的效果
	Ge_Dang(player1,player2);
	return 0;
}
/*------------------------------卡牌效果函数----------------------------------------*/
void Ge_Dang(player &player1,player &player2)
{
	int op,Card_loc;
	while(1)
	{
		printf(" \n%s 的卡牌 格挡 效果触发\n",player1.playername);
		if(!player1.bot)
		printf(" 请选择抽一张牌或者打出一张攻击牌!\n");
		Show_HP(player1);
		if(!player1.bot)
		Show_Hand(player1);
		if(!player1.bot)//人类玩家
		{
			printf(" 1.打出一张攻击牌 2.抽一张牌\n");
			scanf("%d",&op);
		}
		else//电脑玩家
		{
			if(player1.hand.AttackNum!=0)//有攻击牌
			{
				if(player1.deck_inturn.CardNum!=0)//可以抽牌
				op=rand()%2+1;
				else//不能抽牌
				op=1;
			}
			else//无攻击牌
			op=2;
		}
		if(op==2)
		{
			if(player1.deck_inturn.CardNum!=0)//可以抽牌
			{
				if(!player1.bot)
				{
					Drawing(player1);
					break;
				}
				else
				{
					printf(" %s 选择抽牌!\n",player1.playername);
					Bot_Draw(player1);
					break;
				}
			}
			else//无法抽牌
			{
				printf(" 无法抽牌,自动跳过回合\n");
				break;
			}
		}
		else
		{
			if(player1.hand.CardNum!=0)	
			{	
				if(!player1.bot)
				{
					printf(" 请选择出牌或跳过出牌\n");
					printf(" 1.出牌 2.跳过出牌\n");
					scanf("%d", &op);
				}
				else
				op=1;
				if(op==1)
				{
					if(!player1.bot)
					{
						printf(" 请选择你要出的牌 1~%d\n", player1.hand.CardNum);
						scanf("%d", &Card_loc);
						player1.Card_used = player1.hand.card[Card_loc - 1];
					}
					else
					{
						do
						{
							Card_loc=rand()%player1.hand.CardNum;
						} while (!player1.hand.card[Card_loc].Type);//选择攻击牌打出
						player1.Card_used = player1.hand.card[Card_loc];
					}
					if(!player1.Card_used.Type)
					{
						printf(" 选择的非攻击牌,请重新选择!");
						getchar();
						system("cls");
						continue;
					}
					for (int i = Card_loc - 1; i + 1 < player1.hand.CardNum; i++)
					player1.hand.card[i] = player1.hand.card[i + 1];
					player1.hand.CardNum--;
					Level_calculate(player1);
					int damage=player1.Card_used.Level;
					player2.HP-=damage;
					printf(" %s 打出 %s, 对 %s 造成了 %d 点伤害!\n",player1.playername,player1.Card_used.Name,player2.playername,damage);
					break;
				}
			}
			else
			{
				printf(" 无牌可出,自动跳过\n");
				break;
			}
		}
	}
	getchar();
	system("cls");
	return;
}
/*------------------------------机器人函数------------------------------------------*/
int Bot_choose(player &player)
{
	int op;
	while(1)
	{
		Show_Turn(player);
		Show_HP(player);
		//Show_Hand(player);
		if(player.hand.CardNum!=0)//可以出牌
		{
			if(player.deck_inturn.CardNum==0)//无法抽牌
			op=1;//出牌
			else//可以抽牌
			{
				if(player.hand.CardNum<5)//手牌未满
				op=rand()%2+1;//随机出牌或抽牌
				else
				op=1;//出牌
			}
		}
		else//无法出牌
		{
			if(player.deck_inturn.CardNum==0)//无法抽牌
			printf(" 无法出牌,自动跳过回合\n");
			else//可以抽牌
			printf(" 无法出牌,自动抽牌\n");
			op=2;
		}
		if(op==1)
		{
			int Use_flag=Bot_Use(player);
			if(Use_flag==1)//成功使用卡牌
				return 1;//使用卡牌
			else if(Use_flag==2)//无法使用卡牌
			op=2;
			else//取消出牌
			continue;//重新选择
		}
		if(op==2)
		{
			if(player.deck_inturn.CardNum!=0)//抽牌
			{
				if(Bot_Draw(player))
				{
					system("cls");
					return 0;//抽牌
				}
				else
				{
					system("cls");
					continue;//重新开始回合
				}
			}
			else//跳过
			{
				printf(" %s 行动完成!\n",player.playername);	
				getchar();
				system("cls");
				return -1;//跳过回合
			}
		}
	}
}
int Bot_Draw(player &player)
{
	Card drawd;
	int draw_loc;
	draw_loc = rand() % player.deck_inturn.CardNum;
	drawd = player.deck_inturn.card[draw_loc];
	player.deck_inturn.CardNum--;
	for (int j = draw_loc; j + 1 < player.deck_inturn.CardNum; j++)
		player.deck_inturn.card[j] = player.deck_inturn.card[j + 1];
	if (player.hand.CardNum == 5)
	{
		printf(" 手牌已满,卡牌 %s 进入弃牌库!\n", drawd.Name);
		system("cls");
		return -1;
	}
	else
		player.hand.card[player.hand.CardNum] = drawd;
	printf(" %s 行动完成!\n", player.playername);
	if(drawd.Type)
	player.hand.AttackNum++;
	else
	player.hand.DefenceNum++;
	player.hand.CardNum++;
	getchar();
	return 1;
}
int Bot_Use(player &player)
{
	int Card_loc;
	if(player.Turn_mode)//进攻回合
	{
		if(player.hand.AttackNum!=0)//手牌中有攻击牌
		{
			do
			{
				Card_loc=rand()%player.hand.CardNum;
			} while (!player.hand.card[Card_loc].Type);//选择攻击牌打出
			player.Card_used=player.hand.card[Card_loc];
			player.hand.CardNum--;
			for (int i = Card_loc; i< player.hand.CardNum; i++)
			player.hand.card[i] = player.hand.card[i + 1];
			printf(" %s 行动完成!\n", player.playername);
			player.hand.AttackNum--;
			getchar();
			system("cls");
			return 1;//成功使用卡牌
		}
		else//手牌中没有攻击牌
		{
			if(player.deck_inturn.CardNum!=0)//可以抽牌
			{
				printf(" %s  行动完成!\n", player.playername);
				getchar();
				return 2;//抽牌
			}
			else
			{
				printf(" %s  行动完成!\n", player.playername);
				getchar();
				return 2;//跳过
			}
		}
	}
	else//防御回合
	{
		if(player.hand.DefenceNum!=0)//有防御牌
		{
			do
			{
				Card_loc=rand()%player.hand.CardNum;
			} while (player.hand.card[Card_loc].Type);//优先选择防御牌打出
		}
		else//无防御牌
		Card_loc=rand()%player.hand.CardNum;//随机出卡;
		player.Card_used=player.hand.card[Card_loc];
		player.hand.CardNum--;
		for (int i = Card_loc; i< player.hand.CardNum; i++)
		player.hand.card[i] = player.hand.card[i + 1];
		printf(" %s  行动完成!\n", player.playername);
		if(player.Card_used.Type)//打出攻击牌
		player.hand.AttackNum--;
		else
		player.hand.DefenceNum--;
		getchar();
		system("cls");
		return 1;//成功使用卡牌
	}
}
