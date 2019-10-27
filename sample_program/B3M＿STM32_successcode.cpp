  /* USER CODE BEGIN 2 */

uint8_t txdata[9] = {0};

  void pos(unsigned char id,short deg,unsigned short time)
{
    unsigned char deg_l = (unsigned char)(deg)&0x00FF;
    unsigned char deg_h = (unsigned char)(deg>>8)&0x00FF;
    unsigned char time_l = (unsigned char)(time)&0x00FF;
    unsigned char time_h = (unsigned char)(time>>8)&0x00FF;
    unsigned char sum = (0x09+0x06+id+deg_l+deg_h+time_l+time_h)&0x00FF;

    txdata[0]=0x09;   //1 SIZE
    txdata[1]=0x06;   //2 COMMAND
    txdata[2]=0x00;   //3 OPTION
    txdata[3]=id;     //4 ID
    txdata[4]=deg_l;  //5 POS_L
    txdata[5]=deg_h;  //6 POS_H
    txdata[6]=time_l; //7 TIME_L
    txdata[7]=time_h; //8 TIME_H
    txdata[8]=sum;    //9 SUM
    HAL_Delay(0.03);

}

  void write(unsigned char id)
  {
      unsigned char sum = (0x08+0x04+0x00+id+0x00+0x28+0x01)&0x00FF;
     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
     txdata[0]=0x08;   //1 SIZE
     txdata[1]=0x04;   //2 COMMAND
     txdata[2]=0x00;   //3 OPTION
     txdata[3]=id;     //4 ID
     txdata[4]=0x00;  //DATA
     txdata[5]=0x28;  //ADRESS
     txdata[6]=0x01; //COUNT
     txdata[7]=sum;    //9 SUM
     HAL_UART_Transmit(&huart6, txdata, 8, 0xFFFF);
     HAL_Delay(0.03);
     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);

  }




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  write(0x01);

	  pos(0x01,3000,0);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
	  HAL_UART_Transmit(&huart6, txdata, 9, 0xFFFF);
	  HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
	  HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,1);
	  HAL_Delay(1000);

	  pos(0x01,-3000,0);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
	  HAL_UART_Transmit(&huart6, txdata,9, 0xFFFF);
	  HAL_Delay(100);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
	  HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,0);
	  HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}
