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
*函数名:
　　tcpSvr_recv()
*功能: 这是一个回调函数，当一个TCP段到达这个连接时会被调用

*输入:
*输出:
*说明:
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
			}
		}
		
		if(RELAY_STATE_SUM == i)
		{
			tcp_write(gPcbLastConnectFromClient, "recv err", strlen("recv err"), 0);
			tcp_output(gPcbLastConnectFromClient);		
		}
      
		pbuf_free ( p );  //释放该TCP段    
    }
    else if ( ( err == ERR_OK )  &&  ( p == NULL ) )
    {
        tcp_recv ( pcb, NULL );        /* 设置TCP段到时的回调函数 */
        // tcp_err ( pcb, cli_conn_err );
        tcp_close ( pcb );
        gPcbLastConnectFromClient = NULL;
    }
    else if (err != ERR_OK)
    { 
    }
    
	//tcp_close ( pcb );                                           /* 关闭这个连接 */

    err = ERR_OK; 
    return err;
}

/*******************************************************************************
*函数名:
　　devTcpSvr_accept()
*功能:
*输入:
*输出:
*说明:
*******************************************************************************/
static err_t devTcpSvr_accept ( void *arg, struct tcp_pcb *pcb, err_t err )
{
    tcp_setprio ( pcb, TCP_PRIO_MIN );  /* 设置回调函数优先级，当存在几个连接时特别重要，此函数必须调用*/
    tcp_recv ( pcb, tcpSvr_recv );        /* 设置TCP段到时的回调函数 */
	tcp_err ( pcb,   tcpSvr_err );
    //gPcbLastConnectFromClient = pcb;
    //gPubVal.iSvrNoCommCnt = 0;
    //printf("test tcp\n");
    err = ERR_OK;

    return err;
}
/*******************************************************************************
*函数名:
　　taskTcpServer()
*功能:
*输入:
*输出:
*说明:
*******************************************************************************/
void tcpServerInit ( void )
{
    struct tcp_pcb *pcb;
    err_t ret;
    struct ip_addr ipaddr; 

    printf ( "\r\nstart  tcpServerInit " );

    OSTimeDly ( OS_TICKS_PER_SEC );
  
    pcb = tcp_new();   //建立TCP连接           

#if 0
    ret = tcp_bind ( pcb, IP_ADDR_ANY, 5000 );        /* 绑定本地IP地址和端口号 */
#else

    /* 绑定本地IP地址和端口号 */
    //ret = tcp_bind ( pcb, IP_ADDR_ANY,  *(WORD *)&SysParam[ SP_LOCALPORT] ); 
     ret = tcp_bind ( pcb, IP_ADDR_ANY,  80/*TCP_PORT*/ );  

    //printf( "\r\ntcp server bind port %d " , *(WORD *) &SysParam[ SP_LOCALPORT] );
#endif

    if ( ret != ERR_OK )
    {
        printf ( "\r\ntcp_bind ret:0x%x" , ret );
    }
    
    pcb = tcp_listen ( pcb );                                       /* 进入监听状态 */

    if ( pcb == NULL )
    {
        printf ( "\r\ntcp_listen pcb:0x%x" , ( int ) pcb );
    }
    printf ( "\r\ntcp_listen pcb start...." );
    tcp_accept ( pcb, devTcpSvr_accept );                /* 设置有连接请求时的回调函数 */
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
*函数名:
　　sendToLastConnectFromClient()
*功能:
*输入:
*输出:
*说明:
*******************************************************************************/
void sendToLastConnectFromClient ( void * buff , int len )
{
    err_t ret;
    if ( gPcbLastConnectFromClient != NULL )
    {
       // _pl_;
        ret = tcp_write ( gPcbLastConnectFromClient, buff, len , 0 );
        if ( ERR_OK !=  ret )   //发送数据
        {
            printf("ret :0x%x " ,ret);
            //gPcbLastConnectFromClient = NULL;
        }

        if ( tcp_output ( gPcbLastConnectFromClient ) != ERR_OK )
        {
        }
    }
}



