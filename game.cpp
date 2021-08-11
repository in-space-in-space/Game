#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
typedef struct Card//���ƶ���
{
	int Level;//���Ƶȼ�
	int Type;//��������:����\����
	//������:type=1 ������:type=0
	int Kind;//��������:������
	//����kind=0 ��kind=1 ��kind=2 ��kind=3 ��kind=4 ǹkind=5
	int Fun[15];//����Ч��
	//ѹ��fun=0 �ƺ�fun=1 ����fun=2 ����fun=3 ׷��fun=4
	//���fun=5 �ж�fun=6 ��ӡfun=7 ����fun=7 ����fun=8
	char Name[10];//��������
	char text[100];//��������
}Card;
typedef struct Cards//�ƿⶨ��
{
	Card cards[100];
	int NUM;
}Cards;
typedef struct Deck//���ƶ���
{
	Card card[20];
	int CardNum = 0;
}Deck;
typedef struct Hand//���ƶ���
{
	Card card[5];
	int CardNum = 0;
	int AttackNum=0;
	int DefenceNum=0;
}Hand;
typedef struct player//��Ҷ���
{
	int bot=0;
	char playername[10];
	int HP = 10;//�������
	Hand hand;//�������
	Deck deck;//�������
	Deck deck_inturn;//��ʱ����
	int Attack_level_plus = 0;//�����ȼ�+1
	int Defence_level_plus = 0;//�����ȼ�+1
	int Card_level_plus = 0;//���Ƶȼ�+1
	int Poisoned = 0;//�ж�
	int Marked[5];//���
	int Sealed[5];//��ӡ
	int Turn_mode;//�غ�ģʽ,1Ϊ����,0Ϊ����
    Card Card_used;//�ϻغ�ʹ�ÿ���
    int action;//�ϻغ��ж�,1Ϊ����,0Ϊ����
}player;
/*------------------------------���岿��--------------------------------------------*/
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
/*------------------------------����Ч������----------------------------------------*/
void Ge_Dang(player &player1,player &player2);
/*------------------------------��������--------------------------------------------*/
int main()
{
	Cards Cards;
	player player1;
	player player2;
	int op,winner;
	if (Cards_Initialize(Cards))
	{
		printf("   ���ƶ�ȡ���!\n   ���»س���ʼ!\n");
		getchar();
		system("cls");
	}
	else
	{
		printf("   ���ƶ�ȡʧ��!\n");
		getchar();
		return 0;
	}
	{//ѡ���Ƿ�����Խ�����Ϸ
		printf("  �Ƿ�����Զ�ս?\n 1.�� 0.��\n");
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
	strcpy(player1.playername,"���1");
	if(!player2.bot)
	strcpy(player2.playername,"���2");
	else
	strcpy(player2.playername,"���ǵ���");
}
void Deck_Initialize(player &player)//�����ʼ��
{
	srand(time(NULL));
	for (int i = 0; i < 5; i++)
	{
		strcpy(player.deck.card[i].Name, "ȭ");
		strcpy(player.deck.card[i].text, "��");
		player.deck.card[i].Level = 1;
		player.deck.card[i].Kind = 0;
		player.deck.card[i].Type = 1;
	}
	for (int i = 5; i < 7; i++)
	{
		strcpy(player.deck.card[i].Name, "��");
		strcpy(player.deck.card[i].text, "��");
		player.deck.card[i].Level = 2;
		player.deck.card[i].Kind = 0;
		player.deck.card[i].Type = 1;
	}
	for (int i = 7; i < 10; i++)
	{
		strcpy(player.deck.card[i].Name, "��");
		strcpy(player.deck.card[i].text, "ѹ��:��һ���ƻ���һ�Ź�����");
		player.deck.card[i].Level = 2;
		player.deck.card[i].Fun[0] = 1;
		player.deck.card[i].Kind = 0;
		player.deck.card[i].Type = 0;
	}
	player.deck.CardNum = 10;
}
int Cards_Initialize(Cards& Cards)//���ļ��л�ȡ����
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
int PlayTurn(Cards cards, player& player1, player& player2)//��Ϸ�غ�
{
	int TurnNum = 1,Over_flag=0;
	StartTurn(player1, player2);
	while (1)
	{
        if(player1.Turn_mode)
        {
			printf(" ��%d�غ�:\n",TurnNum);
            player1.action=Choose(player1);
			printf(" ��%d�غ�:\n",TurnNum);
			if(!player2.bot)
            player2.action=Choose(player2);
			else
			player2.action=Bot_choose(player2);
        }
        else
        {
			printf(" ��%d�غ�:\n",TurnNum);
            if(!player2.bot)
            player2.action=Choose(player2);
			else
			player2.action=Bot_choose(player2);
			printf(" ��%d�غ�:\n",TurnNum);
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
int StartTurn(player& player1, player& player2)//��Ϸ��ʼǰ���ƺ;������ط�
{	
	Name_Initialize(player1,player2);
	player1.deck_inturn = player1.deck;//�غ���ʹ�õ����Ƹ���
	player2.deck_inturn = player2.deck;
	Start_Drawing(player1);
	Start_Drawing(player2);
	if (First_Attack_Decide())
		player1.Turn_mode = 1;
	else
		player2.Turn_mode = 1;
    if(player1.Turn_mode)
    printf(" %s ����ȹ�!\n",player1.playername);
    else
    printf(" %s ����ȹ�!\n",player2.playername);
    getchar();
	return player1.Turn_mode;
}
int First_Attack_Decide()/*�����ȹ�*/
{
	return rand() % 2;
}
int Start_Drawing(player &player)/*��ʼ����*/
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
int Drawing(player &player)/*���ƽ׶�*/
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
		printf(" ��������,���� %s �������ƿ�!\n", drawd.Name);
		getchar();
		getchar();
		system("cls");
		return -1;
	}
	else
		player.hand.card[player.hand.CardNum] = drawd;
	printf(" �鵽�˿��� %s !\n", drawd.Name);
	if(drawd.Type)
	player.hand.AttackNum++;
	else
	player.hand.DefenceNum++;
	getchar();
	getchar();
	player.hand.CardNum++;
	return 1;
}
int Choose(player &player)/*ѡ�ƽ׶�*/
{
	int op;
	while(1)
	{
		Show_Turn(player);
		Show_HP(player);
		Show_Hand(player);
		if(player.hand.CardNum!=0)//���Գ���
		{
			if(player.deck_inturn.CardNum==0)//�޷�����
			{
				printf(" ��ѡ�����,�����غϻ�����\n");
				printf(" 1.���� 2.�����غ� 0.����\n");
			}
			else//���Գ���
			{
				printf(" ��ѡ�����,���ƻ�����\n");
				printf(" 1.���� 2.���� 0.����\n");
			}
			scanf("%d", &op);
		}
		else//�޷�����
		{
			op=2;
			if(player.deck_inturn.CardNum==0)//�޷�����
			printf(" �޷�����,�Զ������غ�\n");
			else//���Գ���
			printf(" �޷�����,�Զ�����\n");
		}
		if(op==1)
		{
			int Use_flag=Use(player);
			if(Use_flag==1)//�ɹ�ʹ�ÿ���
			{
				system("cls");
				return 1;//ʹ�ÿ���
			}
			else if(Use_flag==2)//�޷�ʹ�ÿ���
			op=2;
			else//ȡ������
			continue;//���¿�ʼ�غ�
		}
		if(op==2)
		{
			if(player.deck_inturn.CardNum!=0)//����
			{
				if(Drawing(player))
				{
					system("cls");
					return 0;//����
				}
				else
				{
					system("cls");
					continue;//���¿�ʼ�غ�
				}
			}
			else//����
			{
				printf(" �������ûغ�!\n");
				getchar();
				system("cls");
				return -1;//�����غ�
			}
		}
		else
		{
			printf(" ������!\n");
			getchar();
			getchar();
			system("cls");
			return 2;//����
		}
	}
}
int Use(player&player)/*���ƽ׶�*/
{
	int Card_loc;
	if(player.Turn_mode)//�����غ�
	{
		if(player.hand.AttackNum!=0)//�������й�����
		{
			printf(" ��ѡ�񹥻��ƴ��! 1~%d\n", player.hand.CardNum);
			printf(" ȡ��ѡ�������� 0\n");
		    scanf("%d",&Card_loc);
			if (!Card_loc)
			{
				getchar();
				getchar();
				system("cls");
				return 0;//����ʹ�ÿ���
			}
			else
			{
				player.Card_used=player.hand.card[Card_loc-1];
				if((!player.Card_used.Type)&&(player.Turn_mode))
                {
                    printf(" ѡ��ķǹ�����,������ѡ��!");
                    getchar();
                    getchar();
                    system("cls");
                    return 0;//ѡ���ƴ���
                }
				for (int i = Card_loc - 1; i + 1 < player.hand.CardNum; i++)
				player.hand.card[i] = player.hand.card[i + 1];
				player.hand.CardNum--;
				printf(" ���� %s ������!\n", player.Card_used.Name);
				player.hand.AttackNum--;
				getchar();
				getchar();
				system("cls");
				return 1;//�ɹ�ʹ�ÿ���
			}
		}
		else//������û�й�����
		{
			if(player.deck_inturn.CardNum!=0)//���Գ���
			{
				printf(" ������û�й�����,�Զ�����!\n");
				return 2;//����
			}
			else
			{
				printf(" ������û�й�����,�޷�����,�Զ������غ�!\n");
				return 2;//����
			}
		}
	}
	else//�����غ�
	{
		printf(" ������ѡ��һ���ƴ��! 1~%d \n",player.hand.CardNum);
		printf(" ȡ��ѡ�������� 0\n");
		scanf("%d",&Card_loc);
		if (!Card_loc)
		{
			getchar();
			system("cls");
			return 0;//����ʹ�ÿ���
		}
		else
		{
			player.Card_used=player.hand.card[Card_loc-1];
			for (int i = Card_loc - 1; i + 1 < player.hand.CardNum; i++)
			player.hand.card[i] = player.hand.card[i + 1];
			player.hand.CardNum--;
			printf(" ���� %s ������!\n", player.Card_used.Name);
			if(player.Card_used.Type)//���������
			player.hand.AttackNum--;
			else
			player.hand.DefenceNum--;
			getchar();
			system("cls");
			return 1;//�ɹ�ʹ�ÿ���
		}
	}
}
void Show_Turn(player player)//��ʾ�غ�ģʽ
{
	printf("\n��ǰΪ %s ��",player.playername);
	if(player.Turn_mode)
	printf(" �����غ�\n");
	else
	printf(" �����غ�\n");
}
void Show_HP(player player)//��ʾѪ��
{
	printf(" ��ǰѪ��: ");
	for (int i = 0; i < 10; i++)
	{
		if (i < player.HP)
			printf("��");
		else
			printf("��");
	}
	printf(" ��%d��\n", player.HP);
}
void Show_Hand(player player)//��ʾ����
{
	printf(" ��ǰ����: ");
	printf("         �ƿ�ʣ�࿨��: %d \n",player.deck_inturn.CardNum);
	if(player.hand.CardNum==0)
	{
		printf(" �� \n");
		return;
	}
	for (int i = 0; i < player.hand.CardNum; i++)
		printf(" %d.%s ", i + 1, player.hand.card[i].Name);
	printf("\n");
}
void Show_Card(Card card)//��ʾ������Ϣ
{
	if(card.Kind==0)
	printf(" ���� ");
	printf(" ���� %s \n",card.Name);
	if(card.Type)
	printf(" �����ȼ�:");
	else
	printf(" �����ȼ�:");
	printf(" %d\n",card.Level);
	printf(" %s\n",card.text);
}
void Show_Deck(Deck deck)//��ʾ������Ϣ
{
	for(int i=0;i<deck.CardNum;i++)
	{
		printf("%d.%-8s",i+1,deck.card[i].Name);
		if(!(i%5))
		printf("\n");
	}
}
int Settlement(player& player1,player& player2)/*����׶�*/
{
    Level_calculate(player1);
    Level_calculate(player2);
	Card_damage(player1,player2);
	Card_function(player1,player2);
    return 0;
}
void Level_calculate(player &player)//���㿨�Ƶȼ�
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
void Card_damage(player& player1,player& player2)//���㿨�Ƶȼ���ɵ��˺�
{
	int damage;
	if(player1.Turn_mode)
    {
        printf(" ������ %s ѡ�� ",player1.playername);
        if(player1.action==1)
        printf("����\n %s ���������:%2s  ���Ƶȼ�:%d  ����Ч��:%s\n\n",player1.playername,player1.Card_used.Name,player1.Card_used.Level,player1.Card_used.text);
        else if(player1.action==0)
        printf("����\n\n");
		else 
		printf("�����غ�\n");
        printf(" ������ %s ѡ�� ",player2.playername);
        if(player2.action==1)
        printf("����\n %s ���������:%2s  ���Ƶȼ�:%d  ����Ч��:%s\n\n",player2.playername,player2.Card_used.Name,player2.Card_used.Level,player2.Card_used.text);
        else if(player2.action==0)
        printf("����\n\n");
		else 
		printf("�����غ�\n");
        printf(" ������:");
        if(player1.action==1&&player2.action==1&&(player1.Card_used.Level>player2.Card_used.Level))
        {
            damage=player1.Card_used.Level-player2.Card_used.Level;
            player2.HP-=damage;
            printf(" %s �� %s ����� %d ���˺�!\n",player1.playername,player2.playername,damage);
        }
		else if(player1.action==1&&player2.action!=1)
		{
			damage=player1.Card_used.Level;
			player2.HP-=damage;
			printf(" %s �� %s ����� %d ���˺�!\n",player1.playername,player2.playername,damage);
		}
        else if(player1.action!=1||player1.Card_used.Level<player2.Card_used.Level)
        {
            player1.Turn_mode=0;
            player2.Turn_mode=1;
            printf(" ���ؽ���!");
        }
		else printf(" ���·���");
		getchar();
		system("cls");
    }
    else
    {
        printf(" ������ %s ѡ�� ",player2.playername);
        if(player2.action==1)
        printf("����\n %s ��������� %4s  ���Ƶȼ� %d  ����Ч�� %s\n",player2.playername,player2.Card_used.Name,player2.Card_used.Level,player2.Card_used.text);
        else if(player2.action==0)
        printf("����\n\n");
		else 
		printf("�����غ�\n");
        printf(" ������ %s ѡ�� ",player1.playername);
        if(player1.action==1)
        printf("����\n %s ��������� %4s  ���Ƶȼ� %d  ����Ч�� %s\n",player1.playername,player1.Card_used.Name,player1.Card_used.Level,player1.Card_used.text);
        else if(player1.action==0)
        printf("����\n\n");
		else 
		printf("�����غ�\n");
		printf(" ������:");
        if(player1.action==1&&player2.action==1&&(player2.Card_used.Level>player1.Card_used.Level))
        {
            damage=player2.Card_used.Level-player1.Card_used.Level;
            player1.HP-=damage;
            printf(" %s �� %s ����� %d ���˺�!\n",player2.playername,player1.playername,damage);
        }
		else if(player2.action==1&&player1.action!=1)
		{
			damage=player2.Card_used.Level;
			player1.HP-=damage;
			printf(" %s �� %s ����� %d ���˺�!\n",player2.playername,player1.playername,damage);
		}
		else if(player2.action!=1||player2.Card_used.Level<player1.Card_used.Level)
        {
            player2.Turn_mode=0;
            player1.Turn_mode=1;
            printf(" ���ؽ���!");
        }
		else printf(" ���·���");
		getchar();
		system("cls");
    }
}
int If_Over(player player1,player player2)//�ж���Ϸ�Ƿ����.
{
	if(player1.action==2&&player2.action!=2)
	return 2;//���2ʤ��
	else if(player2.action==2&&player1.action!=2)
	return 1;//���1ʤ��
	else if(player1.action==2&&player2.action==2)
	return 3;//ƽ��
	if((player1.hand.CardNum==0)&&(player2.hand.CardNum==0)&&(player2.deck_inturn.CardNum==0)&&(player1.deck_inturn.CardNum==0))
	{
		if(player1.HP>player2.HP)
		return 1;//���1ʤ��
		else if(player2.HP>player1.HP)
		return 2;//���2ʤ��
		else
		return 3;//ƽ��
	}
	return 0;//��Ϸδ����
}
int PlayTurn_Over(player player1, player player2, int flag)//��Ϸ�غϽ���
{
	if(flag==1)
	{
		if(player2.action==2)
		printf(" \n%s ����, %s ��ʤ!\n",player2.playername,player1.playername);
		else
		{
			printf(" \n%s ʣ������: %d  \n%s ʣ������: %d \n",player2.playername,player1.HP,player1.playername,player2.HP);
			printf(" \n%s ������, %s ��ʤ\n",player2.playername,player1.playername);
		}
		getchar();
		system("cls");
		return 1;//���1Ӯ
	}
	else if(flag==2)
	{
		if(player1.action==2)
		printf(" \n%s ����, %s ��ʤ!\n",player1.playername,player2.playername);
		else
		{
			printf(" \n%s ʣ������: %d  \n%s ʣ������: %d \n",player1.playername,player1.HP,player1.playername,player2.HP);
			printf(" \n%s ������, %s ��ʤ\n",player1.playername,player2.playername);
		}
		getchar();
		system("cls");
		return 2;//���2Ӯ
	}
	else
	{
		if(player1.action==2)
		printf(" \n˫��ͬʱ����,ƽ��!\n");
		else
		printf(" \n˫��ʣ���������,ƽ��!\n");
		getchar();
		system("cls");
		return 0;//ƽ��
	}
}
int GetCard(Cards &Cards,player &player1,player &player2,int winner)//���ƻ�ȡ����
{
	Card card1,card2;
	int Card_loc1,Card_loc2,op;
	Card_loc1=rand()%Cards.NUM;
	card1=Cards.cards[Card_loc1];
	Card_loc2=rand()%Cards.NUM;
	card2=Cards.cards[Card_loc2];
	if(winner==0)//ƽ��
	{
		{//���1��ÿ���
			printf(" %s �鵽�˿��� %s!\n\n",player1.playername,card1.Name);
			Show_Card(card1);
			if(player1.deck.CardNum<20)//����δ��
			{
				printf("�ѽ� %s ����������!\n",card1.Name);
				player1.deck.card[player1.deck.CardNum]=card1;//��ÿ���
				Cards.NUM--;
				player1.deck.CardNum++;
				for(int i=Card_loc1;i<Cards.NUM;i++)
				Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
			}
			else//��������
			{
				printf(" ��������,��ѡ��һ��������ƽ����������ø���!\n");
				Show_Deck(player1.deck);
				printf(" 1~20.���� 0.����\n");
				scanf("%d",&op);
				if(op==0)
				printf(" ������ÿ��� %s !\n",card1.Name);
				else
				{
					if((strcmp(player1.deck.card[op-1].Name,"ȭ")!=0)&&(strcmp(player1.deck.card[op-1].Name,"��")!=0)&&(strcmp(player1.deck.card[op-1].Name,"��")!=0))//���Ʒǳ�ʼ����
					Cards.cards[Card_loc1]=player1.deck.card[op-1];//�����ƿ��п���
					else;//�����ǳ�ʼ����
					player1.deck.card[op-1]=card1;//��ÿ���
					Cards.NUM--;
					for(int i=Card_loc1;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
				}
			}
			getchar();
			system("cls");
		}
		{//���2��ÿ���
			printf(" %s �鵽�˿��� %s!\n\n",player2.playername,card2.Name);
			Show_Card(card2);
			if(player2.deck.CardNum<20)//����δ��
			{
				printf("�ѽ� %s ����������!\n",card2.Name);
				player2.deck.card[player2.deck.CardNum]=card2;//��ÿ���
				Cards.NUM--;
				player2.deck.CardNum++;
				for(int i=Card_loc2;i<Cards.NUM;i++)
				Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
			}
			else//��������
			{
				printf(" ��������,��ѡ��һ��������ƽ����������ø���!\n");
				Show_Deck(player2.deck);
				printf(" 1~20.���� 0.����\n");
				scanf("%d",&op);
				if(op==0)
				printf(" ������ÿ��� %s !\n",card2.Name);
				else
				{
					if((strcmp(player2.deck.card[op-1].Name,"ȭ")!=0)&&(strcmp(player2.deck.card[op-1].Name,"��")!=0)&&(strcmp(player2.deck.card[op-1].Name,"��")!=0))//���Ʒǳ�ʼ����
					Cards.cards[Card_loc2]=player2.deck.card[op-1];//�����ƿ��п���
					else;//�����ǳ�ʼ����
					player2.deck.card[op-1]=card2;//��ÿ���
					Cards.NUM--;
					for(int i=Card_loc2;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
				}
			}
			getchar();
			system("cls");
		}
	}
	else if(winner==1)//���1ʤ��
	{
		printf(" �鵽�����¿���:\n");
		printf(" ����1: %s ",card1.Name);
		Show_Card(card1);
		printf(" ����2: %s ",card2.Name);
		Show_Card(card2);
		printf(" �� %s ѡ��һ���Ƽ���������,���߷���ѡ��!\n",player1.playername);
		printf(" 1.����1 2.����2 0.����ѡ��\n");
		scanf("%d",&op);
		if(op==0);//����
		else if(op==1)//����1
		{
			{//���1��ÿ���
				printf(" %s ",player1.playername);
				if(player1.deck.CardNum<20)//����δ��
				{
					printf("�ѽ� %s �����������!\n",card1.Name);
					player1.deck.card[player1.deck.CardNum]=card1;//��ÿ���
					Cards.NUM--;
					player1.deck.CardNum++;
					for(int i=Card_loc1;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
				}
				else//��������
				{
					printf(" ��������,��ѡ��һ��������ƽ����������ø���!\n");
					Show_Deck(player1.deck);
					printf(" 1~20.���� 0.����\n");
					scanf("%d",&op);
					if(op==0)
					printf(" ������ÿ��� %s !\n",card1.Name);
					else
					{
						if((strcmp(player1.deck.card[op-1].Name,"ȭ")!=0)&&(strcmp(player1.deck.card[op-1].Name,"��")!=0)&&(strcmp(player1.deck.card[op-1].Name,"��")!=0))//���Ʒǳ�ʼ����
						Cards.cards[Card_loc1]=player1.deck.card[op-1];//�����ƿ��п���
						else;//�����ǳ�ʼ����
						player1.deck.card[op-1]=card1;//��ÿ���
						Cards.NUM--;
						player2.deck.CardNum++;
						for(int i=Card_loc1;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
					}
				}
				getchar();
				getchar();
				system("cls");
			}
			{//���2��ÿ���
				printf(" %s ",player2.playername);
				if(player2.deck.CardNum<20)//����δ��
				{
					printf("�ѽ� %s ����������!\n",card2.Name);
					player2.deck.card[player2.deck.CardNum]=card2;//��ÿ���
					Cards.NUM--;
					player2.deck.CardNum++;
					for(int i=Card_loc2;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
				}
				else//��������
				{
					printf(" ��������,��ѡ��һ��������ƽ����������ø���!\n");
					Show_Deck(player2.deck);
					printf(" 1~20.���� 0.����\n");
					scanf("%d",&op);
					if(op==0)
					printf(" ������ÿ��� %s !\n",card2.Name);
					else
					{
						if((strcmp(player2.deck.card[op-1].Name,"ȭ")!=0)&&(strcmp(player2.deck.card[op-1].Name,"��")!=0)&&(strcmp(player2.deck.card[op-1].Name,"��")!=0))//���Ʒǳ�ʼ����
						Cards.cards[Card_loc2]=player2.deck.card[op-1];//�����ƿ��п���
						else;//�����ǳ�ʼ����
						player2.deck.card[op-1]=card2;//��ÿ���
						Cards.NUM--;
						for(int i=Card_loc2;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
					}
				}
				getchar();
				system("cls");
			}
		}
		else//����2
		{
			{//���1��ÿ���
				printf(" %s ",player1.playername);
				if(player1.deck.CardNum<20)//����δ��
				{
					printf("�ѽ� %s ����������!\n",card2.Name);
					player1.deck.card[player1.deck.CardNum]=card2;//��ÿ���
					Cards.NUM--;
					player1.deck.CardNum++;
					for(int i=Card_loc2;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
				}
				else//��������
				{
					printf(" ��������,��ѡ��һ��������ƽ����������ø���!\n");
					Show_Deck(player1.deck);
					printf(" 1~20.���� 0.����\n");
					scanf("%d",&op);
					if(op==0)
					printf(" ������ÿ��� %s !\n",card2.Name);
					else
					{
						if((strcmp(player1.deck.card[op-1].Name,"ȭ")!=0)&&(strcmp(player1.deck.card[op-1].Name,"��")!=0)&&(strcmp(player1.deck.card[op-1].Name,"��")!=0))//���Ʒǳ�ʼ����
						Cards.cards[Card_loc2]=player1.deck.card[op-1];//�����ƿ��п���
						else;//�����ǳ�ʼ����
						player1.deck.card[op-1]=card2;//��ÿ���
						Cards.NUM--;
						for(int i=Card_loc2;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
					}
				}
				getchar();
				getchar();
				system("cls");
			}
			{//���2��ÿ���
				printf(" %s ",player2.playername);
				if(player2.deck.CardNum<20)//����δ��
				{
					printf("�ѽ� %s ����������!\n",card1.Name);
					player2.deck.card[player2.deck.CardNum]=card1;//��ÿ���
					Cards.NUM--;
					player2.deck.CardNum++;
					for(int i=Card_loc1;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
				}
				else//��������
				{
					printf(" ��������,��ѡ��һ��������ƽ����������ø���!\n");
					Show_Deck(player2.deck);
					printf(" 1~20.���� 0.����\n");
					scanf("%d",&op);
					if(op==0)
					printf(" ������ÿ��� %s !\n",card1.Name);
					else
					{
						if((strcmp(player2.deck.card[op-1].Name,"ȭ")!=0)&&(strcmp(player2.deck.card[op-1].Name,"��")!=0)&&(strcmp(player2.deck.card[op-1].Name,"��")!=0))//���Ʒǳ�ʼ����
						Cards.cards[Card_loc1]=player2.deck.card[op-1];//�����ƿ��п���
						else;//�����ǳ�ʼ����
						player2.deck.card[op-1]=card1;//��ÿ���
						Cards.NUM--;
						for(int i=Card_loc1;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
					}
				}
				getchar();
				getchar();
				system("cls");
			}
		}
	}
	else//���2ʤ��
	{
		printf(" �鵽�����¿���:\n");
		printf(" ����1: %s ",card1.Name);
		Show_Card(card1);
		printf(" ����2: %s ",card2.Name);
		Show_Card(card2);
		printf(" ��ѡ��һ���Ƽ���������,���߷���ѡ��!\n");
		printf(" 1.����1 2.����2 0.����ѡ��\n");
		scanf("%d",&op);
		if(op==0);//����
		else if(op==1)//����1
		{
			{//���2��ÿ���
				printf(" %s ",player2.playername);
				if(player2.deck.CardNum<20)//����δ��
				{
					printf("�ѽ� %s ����������!\n",card1.Name);
					player2.deck.card[player2.deck.CardNum]=card1;//��ÿ���
					Cards.NUM--;
					player2.deck.CardNum++;
					for(int i=Card_loc1;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
				}
				else//��������
				{
					printf(" ��������,��ѡ��һ��������ƽ����������ø���!\n");
					Show_Deck(player2.deck);
					printf(" 1~20.���� 0.����\n");
					scanf("%d",&op);
					if(op==0)
					printf(" ������ÿ��� %s !\n",card1.Name);
					else
					{
						if((strcmp(player2.deck.card[op-1].Name,"ȭ")!=0)&&(strcmp(player2.deck.card[op-1].Name,"��")!=0)&&(strcmp(player2.deck.card[op-1].Name,"��")!=0))//���Ʒǳ�ʼ����
						Cards.cards[Card_loc1]=player2.deck.card[op-1];//�����ƿ��п���
						else;//�����ǳ�ʼ����
						player2.deck.card[op-1]=card1;//��ÿ���
						Cards.NUM--;
						for(int i=Card_loc1;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
					}
				}
				getchar();
				getchar();
				system("cls");
			}
			{//���1��ÿ���
				printf(" %s ",player1.playername);
				if(player1.deck.CardNum<20)//����δ��
				{
					printf("�ѽ� %s ����������!\n",card2.Name);
					player1.deck.card[player1.deck.CardNum]=card2;//��ÿ���
					Cards.NUM--;
					player1.deck.CardNum++;
					for(int i=Card_loc2;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
				}
				else//��������
				{
					printf(" ��������,��ѡ��һ��������ƽ����������ø���!\n");
					Show_Deck(player1.deck);
					printf(" 1~20.���� 0.����\n");
					scanf("%d",&op);
					if(op==0)
					printf(" ������ÿ��� %s !\n",card2.Name);
					else
					{
						if((strcmp(player1.deck.card[op-1].Name,"ȭ")!=0)&&(strcmp(player1.deck.card[op-1].Name,"��")!=0)&&(strcmp(player1.deck.card[op-1].Name,"��")!=0))//���Ʒǳ�ʼ����
						Cards.cards[Card_loc2]=player1.deck.card[op-1];//�����ƿ��п���
						else;//�����ǳ�ʼ����
						player1.deck.card[op-1]=card2;//��ÿ���
						Cards.NUM--;
						for(int i=Card_loc2;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
					}
				}
				getchar();
				system("cls");
			}
		}
		else//����2
		{
			{//���2��ÿ���
				printf(" %s ",player2.playername);
				if(player2.deck.CardNum<20)//����δ��
				{
					printf("�ѽ� %s ����������!\n",card2.Name);
					player2.deck.card[player2.deck.CardNum]=card2;//��ÿ���
					Cards.NUM--;
					player2.deck.CardNum++;
					for(int i=Card_loc2;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
				}
				else//��������
				{
					printf(" ��������,��ѡ��һ��������ƽ����������ø���!\n");
					Show_Deck(player2.deck);
					printf(" 1~20.���� 0.����\n");
					scanf("%d",&op);
					if(op==0)
					printf(" ������ÿ��� %s !\n",card2.Name);
					else
					{
						if((strcmp(player2.deck.card[op-1].Name,"ȭ")!=0)&&(strcmp(player2.deck.card[op-1].Name,"��")!=0)&&(strcmp(player2.deck.card[op-1].Name,"��")!=0))//���Ʒǳ�ʼ����
						Cards.cards[Card_loc2]=player2.deck.card[op-1];//�����ƿ��п���
						else;//�����ǳ�ʼ����
						player2.deck.card[op-1]=card2;//��ÿ���
						Cards.NUM--;
						for(int i=Card_loc2;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
					}
				}
				getchar();
				getchar();
				system("cls");
			}
			{//���1��ÿ���
			printf(" %s ",player1.playername);
				if(player1.deck.CardNum<20)//����δ��
				{
					printf("�ѽ� %s ����������!\n",card1.Name);
					player1.deck.card[player1.deck.CardNum]=card1;//��ÿ���
					Cards.NUM--;
					player1.deck.CardNum++;
					for(int i=Card_loc1;i<Cards.NUM;i++)
					Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
				}
				else//��������
				{
					printf(" ��������,��ѡ��һ��������ƽ����������ø���!\n");
					Show_Deck(player1.deck);
					printf(" 1~20.���� 0.����\n");
					scanf("%d",&op);
					if(op==0)
					printf(" ������ÿ��� %s !\n",card1.Name);
					else
					{
						if((strcmp(player1.deck.card[op-1].Name,"ȭ")!=0)&&(strcmp(player1.deck.card[op-1].Name,"��")!=0)&&(strcmp(player1.deck.card[op-1].Name,"��")!=0))//���Ʒǳ�ʼ����
						Cards.cards[Card_loc1]=player1.deck.card[op-1];//�����ƿ��п���
						else;//�����ǳ�ʼ����
						player1.deck.card[op-1]=card1;//��ÿ���
						Cards.NUM--;
						for(int i=Card_loc1;i<Cards.NUM;i++)
						Cards.cards[i]=Cards.cards[i+1];//���ƿ����Ƴ�����
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
void Card_function(player &player1,player &player2)//����Ч����������
{
	if(player1.action==1&&player2.action==1)//˫��������
	{
		//���1ʹ�õĿ���Ч���ж�--------------------------------------------
		if(player1.Card_used.Fun[1])//ѹ��Ч��
		{
			if(player1.Card_used.Level>player2.Card_used.Level)
			Fun_0_yazhi(player1,player2);
		}

		//���2ʹ�õĿ���Ч���ж�--------------------------------------------
		if(player2.Card_used.Fun[1])//ѹ��Ч��
		{
			if(player2.Card_used.Level>player1.Card_used.Level)
			Fun_0_yazhi(player2,player1);
		}
		return ;
	}
	else//����
	;
}
int Fun_0_yazhi(player &player1,player &player2)//����ѹ��Ч����������
{
	if(!strcmp(player1.Card_used.Name,"��"))// �� ��Ч��
	Ge_Dang(player1,player2);
	return 0;
}
/*------------------------------����Ч������----------------------------------------*/
void Ge_Dang(player &player1,player &player2)
{
	int op,Card_loc;
	while(1)
	{
		printf(" \n%s �Ŀ��� �� Ч������\n",player1.playername);
		if(!player1.bot)
		printf(" ��ѡ���һ���ƻ��ߴ��һ�Ź�����!\n");
		Show_HP(player1);
		if(!player1.bot)
		Show_Hand(player1);
		if(!player1.bot)//�������
		{
			printf(" 1.���һ�Ź����� 2.��һ����\n");
			scanf("%d",&op);
		}
		else//�������
		{
			if(player1.hand.AttackNum!=0)//�й�����
			{
				if(player1.deck_inturn.CardNum!=0)//���Գ���
				op=rand()%2+1;
				else//���ܳ���
				op=1;
			}
			else//�޹�����
			op=2;
		}
		if(op==2)
		{
			if(player1.deck_inturn.CardNum!=0)//���Գ���
			{
				if(!player1.bot)
				{
					Drawing(player1);
					break;
				}
				else
				{
					printf(" %s ѡ�����!\n",player1.playername);
					Bot_Draw(player1);
					break;
				}
			}
			else//�޷�����
			{
				printf(" �޷�����,�Զ������غ�\n");
				break;
			}
		}
		else
		{
			if(player1.hand.CardNum!=0)	
			{	
				if(!player1.bot)
				{
					printf(" ��ѡ����ƻ���������\n");
					printf(" 1.���� 2.��������\n");
					scanf("%d", &op);
				}
				else
				op=1;
				if(op==1)
				{
					if(!player1.bot)
					{
						printf(" ��ѡ����Ҫ������ 1~%d\n", player1.hand.CardNum);
						scanf("%d", &Card_loc);
						player1.Card_used = player1.hand.card[Card_loc - 1];
					}
					else
					{
						do
						{
							Card_loc=rand()%player1.hand.CardNum;
						} while (!player1.hand.card[Card_loc].Type);//ѡ�񹥻��ƴ��
						player1.Card_used = player1.hand.card[Card_loc];
					}
					if(!player1.Card_used.Type)
					{
						printf(" ѡ��ķǹ�����,������ѡ��!");
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
					printf(" %s ��� %s, �� %s ����� %d ���˺�!\n",player1.playername,player1.Card_used.Name,player2.playername,damage);
					break;
				}
			}
			else
			{
				printf(" ���ƿɳ�,�Զ�����\n");
				break;
			}
		}
	}
	getchar();
	system("cls");
	return;
}
/*------------------------------�����˺���------------------------------------------*/
int Bot_choose(player &player)
{
	int op;
	while(1)
	{
		Show_Turn(player);
		Show_HP(player);
		//Show_Hand(player);
		if(player.hand.CardNum!=0)//���Գ���
		{
			if(player.deck_inturn.CardNum==0)//�޷�����
			op=1;//����
			else//���Գ���
			{
				if(player.hand.CardNum<5)//����δ��
				op=rand()%2+1;//������ƻ����
				else
				op=1;//����
			}
		}
		else//�޷�����
		{
			if(player.deck_inturn.CardNum==0)//�޷�����
			printf(" �޷�����,�Զ������غ�\n");
			else//���Գ���
			printf(" �޷�����,�Զ�����\n");
			op=2;
		}
		if(op==1)
		{
			int Use_flag=Bot_Use(player);
			if(Use_flag==1)//�ɹ�ʹ�ÿ���
				return 1;//ʹ�ÿ���
			else if(Use_flag==2)//�޷�ʹ�ÿ���
			op=2;
			else//ȡ������
			continue;//����ѡ��
		}
		if(op==2)
		{
			if(player.deck_inturn.CardNum!=0)//����
			{
				if(Bot_Draw(player))
				{
					system("cls");
					return 0;//����
				}
				else
				{
					system("cls");
					continue;//���¿�ʼ�غ�
				}
			}
			else//����
			{
				printf(" %s �ж����!\n",player.playername);	
				getchar();
				system("cls");
				return -1;//�����غ�
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
		printf(" ��������,���� %s �������ƿ�!\n", drawd.Name);
		system("cls");
		return -1;
	}
	else
		player.hand.card[player.hand.CardNum] = drawd;
	printf(" %s �ж����!\n", player.playername);
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
	if(player.Turn_mode)//�����غ�
	{
		if(player.hand.AttackNum!=0)//�������й�����
		{
			do
			{
				Card_loc=rand()%player.hand.CardNum;
			} while (!player.hand.card[Card_loc].Type);//ѡ�񹥻��ƴ��
			player.Card_used=player.hand.card[Card_loc];
			player.hand.CardNum--;
			for (int i = Card_loc; i< player.hand.CardNum; i++)
			player.hand.card[i] = player.hand.card[i + 1];
			printf(" %s �ж����!\n", player.playername);
			player.hand.AttackNum--;
			getchar();
			system("cls");
			return 1;//�ɹ�ʹ�ÿ���
		}
		else//������û�й�����
		{
			if(player.deck_inturn.CardNum!=0)//���Գ���
			{
				printf(" %s  �ж����!\n", player.playername);
				getchar();
				return 2;//����
			}
			else
			{
				printf(" %s  �ж����!\n", player.playername);
				getchar();
				return 2;//����
			}
		}
	}
	else//�����غ�
	{
		if(player.hand.DefenceNum!=0)//�з�����
		{
			do
			{
				Card_loc=rand()%player.hand.CardNum;
			} while (player.hand.card[Card_loc].Type);//����ѡ������ƴ��
		}
		else//�޷�����
		Card_loc=rand()%player.hand.CardNum;//�������;
		player.Card_used=player.hand.card[Card_loc];
		player.hand.CardNum--;
		for (int i = Card_loc; i< player.hand.CardNum; i++)
		player.hand.card[i] = player.hand.card[i + 1];
		printf(" %s  �ж����!\n", player.playername);
		if(player.Card_used.Type)//���������
		player.hand.AttackNum--;
		else
		player.hand.DefenceNum--;
		getchar();
		system("cls");
		return 1;//�ɹ�ʹ�ÿ���
	}
}
