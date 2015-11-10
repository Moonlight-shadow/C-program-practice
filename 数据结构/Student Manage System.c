#include<stdio.h>
#include<stdlib.h>

typedef struct linknode_s { 
	  int stu_id;             //学生学号
	  int stu_res;            //学生成绩
	  struct linknode_s *next;
} linknode_t;


linknode_t *Createlist()		//创建链表
{
	   int a,b;
	   linknode_t *H,*p,*tmp;
	   H = (linknode_t *)malloc(sizeof(linknode_t));
	   tmp = H;
	   //printf("请录入学生学号和成绩输入-1结束录入\n");
	   /*printf("请录入学生学号：");
	   scanf("%d\n",&a);
	   printf("请录入学生成绩：");
	   scanf("%d",&b);
	   while (a!=-1&&b!=-1)*/
	   do{
	   printf("请录入学生学号：");
       scanf("%d",&a);
	   printf("请录入学生成绩：");
	   scanf("%d",&b);
	   if(b==-1&&a==-1){
			continue;
	   }
	   p = (linknode_t *)malloc(sizeof(linknode_t));
       p->stu_id = a;
	   p->stu_res = b;
	   tmp->next = p;
       tmp = p;
	   }while(a!=-1&&b!=-1);
	   p->next = NULL;	  
	   return H;
}



void VisitLinkList(linknode_t *H)		//遍历链表
{
	linknode_t *p;
	p = H;
	while(p->next != NULL)
	{
		p=p->next;
		printf("%d  %d\n",p->stu_id,p->stu_res);
	}
}



int Getlist(linknode_t *H, int pos)	//获得学生链表第pos个位置学生的成绩
{
	    int j;
		linknode_t *p;
		j = 0;
		p = H;
		//if (pos < 0) {
		//return NULL;
        //}
	    while (p->next && j < pos) {
		p = p->next;
		j++;
		}
		if (pos == j) {
		return p->stu_res;
		} 
		else {
			return NULL;
			}
}

										        

int SearchLinkList(linknode_t *H, int  stu_id)		//查找对应学生学号的成绩
{
	    linknode_t *p;
		p = H->next;
		while (p && p->stu_id != stu_id) {
		p = p->next;
		}
		return p->stu_res;
}

void Insertlist(linknode_t *H, int stu_id, int stu_res)		//插入学生   
{
	    linknode_t *p, *q;
		p=H;
		while(p->next!=NULL){
			p = p->next;
		}
		q = (linknode_t *)malloc(sizeof(linknode_t));
		q->stu_id = stu_id;
		q->stu_res = stu_res;
		p->next = q;
		q->next = NULL;
}


void DeletelistByPos(linknode_t *H, int pos)		//删除链表中第pos位的学生
{
	    linknode_t *p, *q;
		int j;
		j = 0;
		p = H;
	    while (p->next && j < pos-1) {
		p = p->next;
		j++;
		}
		q = p->next;
		p->next = q->next;
		free(q);
}

void DeleteLinkListByVal(linknode_t *H, int stu_id)		//删除链表中学号为stu_id的学生
{
		linknode_t *p,*q;                                                                                                                      
		p = H->next;
		q = H;
		while(p->stu_id != stu_id){
			p = p->next;
			q = q->next;
		}
		q->next = p->next;
		free(p);

}


	int main(){
		linknode_t *Createlist();        //创建链表
		void VisitLinkList(linknode_t *H);       //遍历链表
		
		int Getlist(linknode_t *H, int pos);    //获得学生链表第pos个位置学生的成绩
		int SearchLinkList(linknode_t *H, int  stu_id);      //查找对应学生学号的成绩
		void Insertlist(linknode_t *H, int stu_id, int stu_res);     //插入学生 
		void DeletelistByPos(linknode_t *H, int pos);        //删除链表中第pos位的学生
		void DeleteLinkListByVal(linknode_t *H, int stu_id);   //删除链表中学号为stu_id的学生
 
		while(1){
		
			printf("                      学生成绩管理系统                       \n");
			printf("#############################################################\n");
			printf("#               1.录入学生信息并打印。                      #\n");	
			printf("#               2.查找对应学生学号的成绩。                  #\n");
			printf("#               3.获得学生链表第pos个位置学生的成绩。       #\n");
			printf("#               4.插入学生信息。                            #\n");
			printf("#               5.删除链表中第pos位的学生。                 #\n");
			printf("#               6.删除链表中学号为stu_id的学生。            #\n");
			printf("#############################################################\n");
			printf("请输入您所要做操作所对应的序号:"); 
			

			linknode_t *head;
			int i;
			int res;
			int res1;
			int pos1;
			int stu_id;
			int id;
			int res2;
			int pos2;
			int id2;
			scanf("%d",&i);
			
			
			if(i == 1){
				head = Createlist();
				printf("---------------------------------------------------------\n");
				VisitLinkList(head);
				printf("---------------------------------------------------------\n");
			}
			else if(i == 2){
				printf("---------------------------------------------------------\n");
				printf("请输入你所要查找的学生学号：");
				scanf("%d",&stu_id);
				res = Getlist(head,stu_id);
				printf("该学生成绩为 : %d\n",res);
				printf("---------------------------------------------------------\n");
			}
			else if(i == 3){
				printf("---------------------------------------------------------\n");
				printf("请输入你所要查找的位置 : ");
				scanf("%d",&pos1);
				res1 = SearchLinkList(head,pos1);
				printf("该学生成绩为 : %d\n",res);
				printf("---------------------------------------------------------\n");
			}
			else if(i == 4){
				printf("---------------------------------------------------------\n");
				printf("请输入你要插入的学生信息\n");
				printf("请输入要插入学生的学号：");
				scanf("%d",&id);
				printf("请输入要插入学生的成绩：");
				scanf("%d",&res2);
				Insertlist(head,id,res2);
				VisitLinkList(head);
				printf("---------------------------------------------------------\n");
			}
			else if(i == 5){
				printf("---------------------------------------------------------\n");
				printf("请输入你所要删除的位置：");
				scanf("%d",&pos2);
				DeletelistByPos(head,pos2);
				VisitLinkList(head);
				printf("---------------------------------------------------------\n");
			}
			else if(i == 6){
				printf("---------------------------------------------------------\n");
				printf("请输入你要删除的学生的学号：");
				scanf("%d",&id2);
				DeleteLinkListByVal(head,id2);
				printf("---------------------------------------------------------\n");
			}

		}			
	return 0;
}




