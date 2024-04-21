#include <tl_common.h>
#include <drivers.h>

#define APP_UART_DATA_SIZE 12

typedef struct
{
	unsigned int size;
	unsigned char data[APP_UART_DATA_SIZE];
} app_uart_data_t;

static app_uart_data_t rx_buff = { 0, {0, } };
static app_uart_data_t tx_buff = { 0, {0, } };

void app_uart_init(void)
{
	uart_recbuff_init((unsigned char *)&rx_buff, sizeof(rx_buff));
	uart_gpio_set(UART_TX_PB1, UART_RX_PB7);
	uart_reset(); 
    uart_init(12, 15, PARITY_NONE, STOP_BIT_ONE);
	uart_dma_enable(1, 1);
	irq_set_mask(FLD_IRQ_DMA_EN);
	dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 1);
	uart_irq_enable(0, 0);
}

void app_uart_tx(char * data, u32 size)
{
	while(size > APP_UART_DATA_SIZE)
	{
		memcpy(tx_buff.data, data,  APP_UART_DATA_SIZE);
		data += APP_UART_DATA_SIZE;
		size -= APP_UART_DATA_SIZE;
		tx_buff.size = APP_UART_DATA_SIZE;
		uart_dma_send((unsigned char*)&tx_buff);
		tx_buff.size = 0;
		WaitMs(2);
	}
	if(size > 0)
	{
		memcpy(tx_buff.data, data, size);
		tx_buff.size = size;
		uart_dma_send((unsigned char*)&tx_buff);
		tx_buff.size = 0;
		WaitMs(2);
	}
}

void app_uart_irq(void)
{
	unsigned char status = dma_chn_irq_status_get();

	if (status & FLD_DMA_CHN_UART_RX)
	{
        app_uart_tx(rx_buff.data, rx_buff.size);
		rx_buff.size = 0;
		dma_chn_irq_status_clr(FLD_DMA_CHN_UART_RX);
	}

	if (status & FLD_DMA_CHN_UART_TX)
	{
		dma_chn_irq_status_clr(FLD_DMA_CHN_UART_TX);
	}
}