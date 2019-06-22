#include "includes.h"
#include <dev.h>
#include <SCom.h>
#include <comm.h>
#include <err.h>
#include <ip_addr.h>
#include <tcp.h>
#include "DLT645.h"
#include "devTcpip.h"
#include "app.h"
#include "BSP.h"
#include "stm32f10x_gpio.h"

/* global variable */
unsigned int debugFlag = 1<<2;
char tcpRecvBuf[100];
struct tcp_pcb * gPcbLastConnectFromClient = NULL;
static err_t tcpSvr_err ( void *arg,  err_t err );

const char relay_tcp[6][20] = {
	"relay one on",
	"relay one off",
	"relay two on",
	"relay two off",
	"relay three on",
	"relay three off",
};

const char relay_tcp_back[6][20] = {
	"relay one on done",
	"relay one off done",
	"relay two on done",
	"relay two off done",
	"relay three on done",
	"relay three off done",
};

/*******************************************************************************
*������:
����tcpSvr_recv()
*����: ����һ���ص���������һ��TCP�ε����������ʱ�ᱻ����

*����:
*���:
*˵��:
*******************************************************************************/
static err_t tcpSvr_recv ( void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err )
{	/* todo: needs optimize, aaron.gao */
	unsigned char i = 0;
	
    if ( ( err == ERR_OK ) && ( p != NULL ))
    {
        gPcbLastConnectFromClient = pcb;
        /* Inform TCP that we have taken the data. */
        tcp_recved ( pcb, p->tot_len );
#if 0        
        if ( debugFlag & 0x01 )
        {
            printf ( "\r\n p->len %d/%d " , p->len, p->tot_len );
            printf ( "msg:[%s]" , p->payload );																		
        }
#endif
        memcpy ( tcpRecvBuf , p->payload , p->len );
		
		if(0 == strncmp(tcpRecvBuf, relay_tcp[RELAY_ONE_ON], p->len))
		{
			BSP_Relay_ON(GPIO_Pin_11);
			tcp_write(gPcbLastConnectFromClient, relay_tcp_back[RELAY_ONE_ON], strlen(relay_tcp_back[RELAY_ONE_ON]), 0);
			tcp_output(gPcbLastConnectFromClient);			
		}
		else if(0 == strncmp(tcpRecvBuf, relay_tcp[RELAY_ONE_OFF], p->len))
		{
			BSP_Relay_OFF(GPIO_Pin_11);
			tcp_write(gPcbLastConnectFromClient, relay_tcp_back[RELAY_ONE_OFF], strlen(relay_tcp_back[RELAY_ONE_OFF]), 0);
			tcp_output(gPcbLastConnectFromClient);	
		}
		else if(0 == strncmp(tcpRecvBuf, relay_tcp[RELAY_TWO_ON], p->len))
		{
			BSP_Relay_ON(GPIO_Pin_12);
			tcp_write(gPcbLastConnectFromClient, relay_tcp_back[RELAY_TWO_ON], strlen(relay_tcp_back[RELAY_TWO_ON]), 0);
			tcp_output(gPcbLastConnectFromClient);	
		}
		else if(0 == strncmp(tcpRecvBuf, relay_tcp[RELAY_TWO_OFF], p->len))
		{
			BSP_Relay_OFF(GPIO_Pin_12);
			tcp_write(gPcbLastConnectFromClient, relay_tcp_back[RELAY_TWO_OFF], strlen(relay_tcp_back[RELAY_TWO_OFF]), 0);
			tcp_output(gPcbLastConnectFromClient);	
		}
		else if(0 == strncmp(tcpRecvBuf, relay_tcp[RELAY_THREE_ON], p->len))
		{
			BSP_Relay_ON(GPIO_Pin_13);
			tcp_write(gPcbLastConnectFromClient, relay_tcp_back[RELAY_THREE_ON], strlen(relay_tcp_back[RELAY_THREE_ON]), 0);
			tcp_output(gPcbLastConnectFromClient);	
		}
		else if(0 == strncmp(tcpRecvBuf, relay_tcp[RELAY_THREE_OFF], p->len))
		{
			BSP_Relay_OFF(GPIO_Pin_13);
			tcp_write(gPcbLastConnectFromClient, relay_tcp_back[RELAY_THREE_OFF], strlen(relay_tcp_back[RELAY_THREE_OFF]), 0);
			tcp_output(gPcbLastConnectFromClient);	
		}
		else
		{
			tcp_write(gPcbLastConnectFromClient, "recv err", strlen("recv err"), 0);
			tcp_output(gPcbLastConnectFromClient);
		}
        
		pbuf_free ( p );  //�ͷŸ�TCP��    
    }
    else if ( ( err == ERR_OK )  &&  ( p == NULL ) )
    {
        DBMTR( "\r\n close socket in recv \r\n" );
        tcp_recv ( pcb, NULL );        /* ����TCP�ε�ʱ�Ļص����� */
        // tcp_err ( pcb, cli_conn_err );
        tcp_close ( pcb );
        gPcbLastConnectFromClient = NULL;
    }
    else if (err != ERR_OK)
    {
        DBMTR("");    
    }
    
	//tcp_close ( pcb );                                           /* �ر�������� */

    err = ERR_OK; 
    return err;
}

/*******************************************************************************
*������:
����devTcpSvr_accept()
*����:
*����:
*���:
*˵��:
*******************************************************************************/
static err_t devTcpSvr_accept ( void *arg, struct tcp_pcb *pcb, err_t err )
{
    if ( debugFlag & 0x01 )
        printf ( "\r\n tcp connect accepted.. " );

    tcp_setprio ( pcb, TCP_PRIO_MIN );  /* ���ûص��������ȼ��������ڼ�������ʱ�ر���Ҫ���˺����������*/
    tcp_recv ( pcb, tcpSvr_recv );        /* ����TCP�ε�ʱ�Ļص����� */
	tcp_err ( pcb,   tcpSvr_err );
    //gPcbLastConnectFromClient = pcb;
    //gPubVal.iSvrNoCommCnt = 0;
    //printf("test tcp\n");
    err = ERR_OK;

    return err;
}
/*******************************************************************************
*������:
����taskTcpServer()
*����:
*����:
*���:
*˵��:
*******************************************************************************/
void tcpServerInit ( void )
{
    struct tcp_pcb *pcb;
    err_t ret;
    struct ip_addr ipaddr; 

    printf ( "\r\nstart  tcpServerInit " );

    OSTimeDly ( OS_TICKS_PER_SEC );
  
    pcb = tcp_new();   //����TCP����           

#if 0
    ret = tcp_bind ( pcb, IP_ADDR_ANY, 5000 );        /* �󶨱���IP��ַ�Ͷ˿ں� */
#else

    /* �󶨱���IP��ַ�Ͷ˿ں� */
    //ret = tcp_bind ( pcb, IP_ADDR_ANY,  *(WORD *)&SysParam[ SP_LOCALPORT] ); 
     ret = tcp_bind ( pcb, IP_ADDR_ANY,  80/*TCP_PORT*/ );  

    //printf( "\r\ntcp server bind port %d " , *(WORD *) &SysParam[ SP_LOCALPORT] );
#endif

    if ( ret != ERR_OK )
    {
        DBMTR("");
        printf ( "\r\ntcp_bind ret:0x%x" , ret );
    }
    
    pcb = tcp_listen ( pcb );                                       /* �������״̬ */

    if ( pcb == NULL )
    {
        DBMTR("");
        printf ( "\r\ntcp_listen pcb:0x%x" , ( int ) pcb );
    }
    printf ( "\r\ntcp_listen pcb start...." );
    tcp_accept ( pcb, devTcpSvr_accept );                /* ��������������ʱ�Ļص����� */

}


static err_t tcpSvr_err ( void *arg,  err_t err )
{
    printf("\r\n *** tcpSvr_err()**** " );
    
    if (gPcbLastConnectFromClient != NULL)
    {
        tcp_close(gPcbLastConnectFromClient);
        gPcbLastConnectFromClient = NULL;
    }
    return ERR_OK;
}


/*******************************************************************************
*������:
����sendToLastConnectFromClient()
*����:
*����:
*���:
*˵��:
*******************************************************************************/
void sendToLastConnectFromClient ( void * buff , int len )
{
    err_t ret;
    if ( gPcbLastConnectFromClient != NULL )
    {
       // _pl_;
        ret = tcp_write ( gPcbLastConnectFromClient, buff, len , 0 );
        if ( ERR_OK !=  ret )   //��������
        {
            DBMTR("");
            printf("ret :0x%x " ,ret);
            //gPcbLastConnectFromClient = NULL;
        }

        if ( tcp_output ( gPcbLastConnectFromClient ) != ERR_OK )
        {
            DBMTR("");
        }
    }


}



