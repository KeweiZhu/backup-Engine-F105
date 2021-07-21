/************************************************************************************
模式判断
判断逻辑：主要分为两部分，第一部分是控制模式，分为遥控模式，键盘模式，掉电模式，
自动登岛模式以及自动取弹模式
第二部分为控制目标，针对遥控模式时遥控的部分，比如底盘移动，底盘升降，上层升降，
上层伸缩，救援机械手等等，遥控器状态有限，随时修改

s1(右)：在下{s2（左）在下：断电模式
             s2（左）在上：救援模式}
s1（右）：在中{s2（左）在下：底盘模式
               s2（左）在中：手动上岛模式
               s2（左）在上：自动上岛模式}						 
s1（右）：在上{s2（左）在下：上升模式
               s2（左）在中：手动取弹模式
               s2（左）在上：自动取弹模式}
***************************************************************************************/
#include "state_task.h"

key_control  key;
uint32_t  Offline_Check_high_water;
uint32_t  Mode_Switch_high_water;
/**
  * @brief  线路断线检测
  * @param  None
  * @retval 返回断线id
  */

u8 offline_detect(void)
{
	g_flag.offline_flag  = 0;
/**************CAN1断线检测*******************/
	if (g_flag.can1_connect == 0)
		g_flag.can1_cnt ++;
	
	if (g_flag.can1_cnt > 100)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_13 );
		g_flag.offline_flag |= 0x01;
	}
/**************CAN2断线检测*******************/    
    if (g_flag.can2_connect == 0)
        g_flag.can2_cnt ++;
    
    if (g_flag.can2_cnt > 100)
		{
			  GPIO_ResetBits(GPIOC,GPIO_Pin_13 );
			  g_flag.offline_flag |= 0x02;
		}
		else 
	     GPIO_SetBits(GPIOC,GPIO_Pin_13 );
/**************遥控器断线检测*******************/
//    if (g_flag.rc_connect == 0)
//        g_flag.rc_cnt ++;
//    
//    if (g_flag.rc_cnt > 200)
//	{
//        UART5_Configuration();        //重启遥控器串口
//		return 3;
//	}

/**************陀螺仪断线检测*******************/
	if (g_flag.gyro_connect == 0)
		g_flag.gyro_cnt ++;
	
	if (g_flag.gyro_cnt > 50)
	{
		g_flag.gyro_use_flag = 0;    //掉线
		g_flag.offline_flag |= 0x04;
	}
//	else
//		g_flag.gyro_use_flag = 1;    //数据正常
/**************数据正常*******************/
	return g_flag.offline_flag;
}

int offline_state;
void  Offline_Check_task(void *pvParameters)
{
	while(1){
		
		offline_state = offline_detect();
		
				vTaskDelay(1000);
		#if INCLUDE_uxTaskGetStackHighWaterMark
        Offline_Check_high_water = uxTaskGetStackHighWaterMark(NULL);
#endif
    }
}

/**
  * @brief  模式判断
  * @param  None
* @retval s1: 1:键鼠模式
			  2:断电模式
			  3:遥控模式
  */
void switch_mode(void)
{
	
		Control_State_Get();
		Motion_Target_Get();																	//获取运动模式
		
//	if (rc_ctrl.rc.s1 == 2)            
//	{
//		      
//		      if(rc_ctrl.rc.s2 == 1)//键鼠模式
//					{
//						  g_flag.control_mode = KEY_MODE;
//								
//					}
//					  else if(rc_ctrl.rc.s2 == 3)//救援模式
//					{
//						g_flag.control_mode = SAVE_MODE;
//					}
//					 else if (rc_ctrl.rc.s2 == 2)   //断电模式
//						 {
//							 g_flag.control_mode = POWER_OFF_MODE;
//							 key_control_Init();

//						 }
//	}
//	else if (rc_ctrl.rc.s1 == 3)      
//	{
//		        
//           if(rc_ctrl.rc.s2 == 3) //手动上岛模式
//						 {	
//						 g_flag.control_mode = MANUAL_LAND_MODE;

//						 }
//					else  if (rc_ctrl.rc.s2 == 2)//遥控模式
//					 {
//						 g_flag.control_mode = RC_MODE;
//		         g_flag.landing_state = STATE0;
//						 key.flag_lift_up_mode = 0;
//						 key.flag_key_mode = 0;
//						 key_control_Init();
//					 }
//			      else if (rc_ctrl.rc.s2 == 1) //测试自动上岛
//		        {
//				      g_flag.control_mode = LANDING_ON_MODE;

//			      }
//	}
//	else if (rc_ctrl.rc.s1 == 1)      
//	{
//		           
//		           if (rc_ctrl.rc.s2 == 1)   //测试自动取弹
//		           {
//								 g_flag.control_mode = GET_BULLET_MODE;
//								 g_flag.landing_state = STATE0;

//		            }
//		           else if(rc_ctrl.rc.s2 == 3)//手动取弹模式
//						  {
//							  g_flag.control_mode = MANUAL_GET_BULLET_MODE;

//						  }
//		
//		           else if (rc_ctrl.rc.s2 == 2)   //上升    
//		           {
//			           g_flag.control_mode = LIFT_UP_MODE;
//		           }
//	}
//	
//	
//	        
//						  if(rc_ctrl.key.b == 1 && rc_ctrl.key.ctrl == 1) //手动上岛模式
//							  {	
//								  key_control_Init();
//								  key.flag_manual_landing_mode = 1;
//							  }
//							else if (rc_ctrl_last.key.b == 0 && rc_ctrl.key.b == 1) //测试自动上岛
//							  {
//								  key_control_Init();
//									key.flag_landing_mode = 1;
//								
//						   	}
//								else if(rc_ctrl.key.r == 1 && rc_ctrl.key.ctrl == 1)//手动取弹模式
//								 {
//								  	key_control_Init();
//                    key.flag_manual_get_buttel_mode = 1;
//								 }
//			
//							else if (rc_ctrl_last.key.r == 0 && rc_ctrl.key.r == 1 )   //测试自动取弹
//								 {
//								  	key_control_Init();
//									 	key.flag_get_buttel_mode = 1;

//									}
//							else if (rc_ctrl.key.c == 1&&rc_ctrl.key.ctrl == 1)   //下降   
//								 {
//										key_control_Init();

//									  
//								 }
//							else if (rc_ctrl_last.key.c == 0 && rc_ctrl.key.c == 1)   //上升    
//								 {
//										key_control_Init();
//									 	key.flag_lift_up_mode = 1;
//									  
//								 }
//							else if (rc_ctrl_last.key.v == 0 && rc_ctrl.key.v == 1)//救援
//								 {
//									 key_control_Init();
//								   key.flag_save_mode = 1;
//								 } 
//							else if (rc_ctrl.key.e == 1&&rc_ctrl.key.shift == 1)//初始化
//							{
//								key_control_Init();
//								key.flag_initialize_mode = 1;
//							}
//							else if (rc_ctrl_last.key.e == 0 && rc_ctrl.key.e == 1)//键鼠
//							   {
//									 key_control_Init();
//								   key.flag_key_mode = 1;
//								 }
//								  

//								 

//				if(key.flag_landing_mode == 1)
//				{
//					 g_flag.control_mode = LANDING_ON_MODE;

//				}
//				else if(key.flag_manual_landing_mode == 1)
//				{

//					 g_flag.control_mode = MANUAL_LAND_MODE;

//				}
//				else if(key.flag_get_buttel_mode == 1)
//				{

//					g_flag.control_mode = GET_BULLET_MODE;

//				}
//				else if(key.flag_get_buttel_mode1 == 1)
//				{

//					g_flag.control_mode = GET_BULLET_MODE1;	
//					
//				}
//				else if(key.flag_manual_get_buttel_mode == 1)
//				{
//					g_flag.control_mode = MANUAL_GET_BULLET_MODE;

//				}
//				else if(key.flag_key_mode == 1)
//				{

//					g_flag.control_mode = KEY_MODE;
//					
//				}
//				else if(key.flag_lift_up_mode == 1)
//				{
//					g_flag.control_mode = LIFT_UP_MODE;
//					
//				}
//				else if(key.flag_save_mode == 1)
//				{

//					g_flag.control_mode = SAVE_MODE;
//				}
//				else if(key.flag_initialize_mode == 1)
//				{
//					g_flag.control_mode = INITIALIZE_MODE;
//				}
//				else if(key.flag_choose_mode== 1)
//				{
//					g_flag.control_mode = CHOOSE_MODE;
//					
//				}
				
}

void Control_State_Get(void)															//获取控制模式
{
	if (rc_ctrl.rc.s2 == DOWN)															//获取控制模式
	{
		switch(rc_ctrl.rc.s1)
		{
			case UP:
				g_flag.control_mode = RC_MODE;				//遥控模式
				break;
			case MIDDLE:
				g_flag.control_mode = KEY_MODE;				//键鼠模式
				break;
			case DOWN:
				g_flag.control_target = POWER_OFF_MODE;	//掉电模式
				break;
			default:
				break;
		}
	}
}

void Motion_Target_Get(void)															//获取运动模式		
{
	if(g_flag.control_mode == RC_MODE)				//遥控器模式
	{
		if (rc_ctrl.rc.s2 == MIDDLE)			//底盘模式
		{
			switch(rc_ctrl.rc.s1)
			{
				case UP:						//正常底盘运动
					g_flag.control_target = CHASSIS_MODE;	
					break;
				case MIDDLE:				//底盘静步模式
					g_flag.control_target = CHASSIS_MODE_STATIC;
					break;
				case DOWN:					//底盘模式3
					g_flag.control_target = CHASSIS_MODE3;
					break;
				default:
					break;
			}			
		}
		
		else if (rc_ctrl.rc.s2 == UP)			//上层模式
		{
			switch(rc_ctrl.rc.s1)
			{
				case UP:						//上层抬升模式
					g_flag.control_target = SENIOR_UP_MODE;
					break;
				case MIDDLE:				//上层模式2
					g_flag.control_target = SENIOR_MODE2;
					break;
				case DOWN:					//上层模式3
					g_flag.control_target = SENIOR_MODE3;
					break;
				default:
					break;
			}						
		}
	}else if (g_flag.control_mode == KEY_MODE)	//键鼠模式
	{
		//掉电模式
		if(rc_ctrl.key.ctrl == 1 && rc_ctrl.key.b == 1)
		{
			g_flag.control_target = POWER_OFF_MODE;
		}
		
		//底盘模式
		if(rc_ctrl.key.ctrl == 1 && rc_ctrl.key.z == 1)
		{
			g_flag.control_target = CHASSIS_MODE;
		}
		if(rc_ctrl.key.ctrl == 1 && rc_ctrl.key.x == 1)
		{
			g_flag.control_target = CHASSIS_MODE_STATIC;
		}
		if(rc_ctrl.key.ctrl == 1 && rc_ctrl.key.c == 1)
		{
			g_flag.control_target = CHASSIS_MODE3;
		}
		
		//上层模式
		if(rc_ctrl.key.ctrl == 1 && rc_ctrl.key.q == 1)
		{
			g_flag.control_target = SENIOR_UP_MODE;
		}
		if(rc_ctrl.key.ctrl == 1 && rc_ctrl.key.e == 1)
		{
			g_flag.control_target = SENIOR_MODE2;
		}
		if(rc_ctrl.key.ctrl == 1 && rc_ctrl.key.r == 1)
		{
			g_flag.control_target = SENIOR_MODE3;
		}
		
	}
}



void  key_control_Init(void)
{
	key.flag_save_mode = 0;
	key.flag_landing_mode = 0;
	key.flag_manual_landing_mode = 0;
	key.flag_get_buttel_mode = 0;
	key.flag_manual_get_buttel_mode = 0;
	key.flag_key_mode = 0;
	key.flag_lift_up_mode = 0;
  key.flag_initialize_mode = 0;
	key.flag_get_buttel_mode1 = 0;
	key.flag_choose_mode = 0;
}

void Mode_Switch_task(void *pvParameters)
{
	while(1){
		
		switch_mode();
		
		vTaskDelay(10);
		#if INCLUDE_uxTaskGetStackHighWaterMark
        Mode_Switch_high_water = uxTaskGetStackHighWaterMark(NULL);
#endif
    }
}
