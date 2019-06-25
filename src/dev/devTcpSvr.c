#include "includes.h"
#include <err.h>
#include "tcp.h"
//#include "app.h"
//#include "BSP.h"

/* todo: i am so confused about why i cannot use bsp.h and app.h */
#define RELAY_STATE_SUM		(6UL)

/* global variable */
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
{	
	/* todo: needs optimize, aaron.gao */
	unsigned char  i = 0;
	
    if ( ( err == ERR_OK ) && ( p != NULL ))
    {
        gPcbLastConnectFromClient = pcb;
        /* Inform TCP that we have taken the data. */
        tcp_recved ( pcb, p->tot_len );

        memcpy ( tcpRecvBuf , p->payload , p->len );
		
		for(i = 0; i < RELAY_STATE_SUM; i++)
		{
			if(0 == strncmp(tcpRecvBuf, relay_tcp[i], p->len))
			{
				BSP_Relay_Con(i);
				tcp_write(gPcbLastConnectFromClient, relay_tcp_back[i], strlen(relay_tcp_back[i]), 0);
				tcp_output(gPcbLastConnectFromClient);
				break;
			}
		}
		
		if(RELAY_STATE_SUM == i)
		{
			tcp_write(gPcbLastConnectFromClient, "recv err", strlen("recv err"), 0);
			tcp_output(gPcbLastConnectFromClient);		
		}
      
		pbuf_free ( p );  //�ͷŸ�TCP��    
    }
    else if ( ( err == ERR_OK )  &&  ( p == NULL ) )
    {
        tcp_recv ( pcb, NULL );        /* ����TCP�ε�ʱ�Ļص����� */
        // tcp_err ( pcb, cli_conn_err );
        tcp_close ( pcb );
        gPcbLastConnectFromClient = NULL;
    }
    else if (err != ERR_OK)
    { 
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
        printf ( "\r\ntcp_bind ret:0x%x" , ret );
    }
    
    pcb = tcp_listen ( pcb );                                       /* �������״̬ */

    if ( pcb == NULL )
    {
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
            printf("ret :0x%x " ,ret);
            //gPcbLastConnectFromClient = NULL;
        }

        if ( tcp_output ( gPcbLastConnectFromClient ) != ERR_OK )
        {
        }
    }
}



