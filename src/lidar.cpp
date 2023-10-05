#include "../include/lidar.hpp"


void lidar_a_Task(void *p)
{
    while(1)
    {
        Serial.printf("Hello World\r\n");
        delay(10);
    }

}

lidar_t::lidar_t(String Node_Name,
                 char* Wifi_SSID,
                 char* Pass_Word,
                 String Host_Ip,
                 uint16_t Port,
                 uint8_t Serial_Num)
{
        Ros_Serial_Init(Serial_Num);

        Wifi_Init(Wifi_SSID,Pass_Word,Host_Ip,Port,2000);

        Ros_Init(Node_Name);

        // 初始化LED
        pinMode(LED_Pin, OUTPUT);

        digitalWrite(LED_Pin, HIGH);

        /**
         * @brief 创建一个任务在Core 0 上
         * microros_task    任务函数
         * "microros_task"  任务名称
         * 10240      任务占用内存大小
         * NULL         任务参数，为空
         * 1               任务优先级
         * NULL     任务Handle可以为空
         * 0                 内核编号
         */
        xTaskCreatePinnedToCore([](void*param)->void
                                {lidar_t *I = static_cast<lidar_t*>(param);
                                 I->Micro_Ros_Task();},
                                "microros_task", 
                                64*1024, 
                                this, 
                                1, 
                                NULL, 
                                0);
        /**
         * @brief 创建一个任务在Core 0 上
         * lidar_Data_Task    任务函数
         * "lidar_Data_Task"  任务名称
         * 1024      任务占用内存大小
         * this         任务参数，为空
         * 1               任务优先级
         * NULL     任务Handle可以为空
         * 0                 内核编号
         */
        xTaskCreatePinnedToCore([](void*param)->void
                                {lidar_t *I = static_cast<lidar_t*>(param);
                                 I->lidar_Data_Task();},
                                "lidar_Data_Task", 
                                1024, 
                                this, 
                                1, 
                                NULL, 
                                0);
}

lidar_t::lidar_t(uint8_t Serial_Num)
{
        Ros_Serial_Init(Serial_Num);

        // 初始化LED
        //pinMode(LED_Pin, OUTPUT);

        //digitalWrite(LED_Pin, HIGH);
        /**
         * @brief 创建一个任务在Core 0 上
         * lidar_Data_Task    任务函数
         * "lidar_Data_Task"  任务名称
         * 1024      任务占用内存大小
         * this         任务参数，为空
         * 1               任务优先级
         * NULL     任务Handle可以为空
         * 0                 内核编号
         */
        xTaskCreatePinnedToCore([](void*param)->void
                                {lidar_t *I = static_cast<lidar_t*>(param);
                                 I->lidar_Data_Task();},
                                "lidar_Data_Task", 
                                1536, 
                                this, 
                                1, 
                                NULL, 
                                0);
        // xTaskCreatePinnedToCore(lidar_a_Task,
        //                         "lidar_a_Task", 
        //                         10240, 
        //                         NULL, 
        //                         1, 
        //                         NULL, 
        //                         0);
}

void lidar_t::Ros_Init(String Node_Name)
{
    // 初始化内存分配器
    this->allocator = rcl_get_default_allocator();
    // 创建初始化选项
    rclc_support_init(&this->support, 0, NULL, &this->allocator);
    // 创建节点 microros_wifi
    rclc_node_init_default(&this->node, "microros_wifi", "", &this->support);
    //定义一个订阅者，并将他的名称定义为led_control
    rclc_subscription_init_default(
        &this->subscriber,
        &this->node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
        Node_Name.c_str());
        
    // 创建执行器
    rclc_executor_init(&this->executor, &this->support.context, 1, &this->allocator);
    // 为执行器添加一个订阅者
    rclc_executor_add_subscription(&this->executor, &this->subscriber, &this->sub_msg, &lidar_t::callback_subscription_, ON_NEW_DATA);
}

void lidar_t::Wifi_Init(char* Wifi_SSID,
                        char* Pass_Word,
                        String Host_Ip,
                        uint16_t Port,
                        uint16_t Wait_Time)
{
    // 设置通过WIFI进行MicroROS通信
    IPAddress agent_ip;
    agent_ip.fromString(Host_Ip.c_str());
    // 设置wifi名称，密码，电脑IP,端口号
    set_microros_wifi_transports(Wifi_SSID, Pass_Word, agent_ip, Port);
    delay(Wait_Time);
}
