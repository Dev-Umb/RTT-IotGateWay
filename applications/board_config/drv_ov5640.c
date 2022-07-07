#ifndef __DRV_OV5640__
#define __DEV_OV5640__
#include <board_config.h>
#if CAMERA_USING_OV5640
#include <rtdevice.h>
#include "drv_lcd.h"
#include <board.h>
#include <gpiohs.h>
#include "dvp.h"
#include "plic.h"
#include <drv_io_config.h>
#include <rthw.h>

#define DBG_TAG  "LCD"
#define DBG_LVL  DBG_WARNING
#include <rtdbg.h>

#define OV5640_ID           0X5640
#define OV5640_ADDR         0X78
#define OV5640_CHIPIDH      0X300A
#define OV5640_CHIPIDL      0X300B

#define XSIZE               320
#define YSIZE               240
#define LCD_GRAM_ADDRESS    0x60020000

#define QQVGA_160_120       0
#define QCIF_176_144        1
#define QVGA_320_240        2
#define WQVGA_400_240       3
#define CIF_352_288         4

#define jpeg_buf_size       (30*1024)

static struct rt_device _ov5640;
const static uint16_t ov5640_config[][2] = {
	{0x3103, 0x11},
    {0x3008, 0x82},
    {0x3008, 0x42},
	{0x3103, 0x03},

	{0x3035, 0x21},
	{0x3036, 90},
	{0x3037, 0x12},
	{0x3108, 0x01},

	{0x3630, 0x36},
	{0x3631, 0x0e},
	{0x3632, 0xe2},
	{0x3633, 0x12},
	{0x3621, 0xe0},
	{0x3704, 0xa0},
	{0x3703, 0x5a},
	{0x3715, 0x78},
	{0x3717, 0x01},
	{0x370b, 0x60},
	{0x3705, 0x1a},
	{0x3905, 0x02},
	{0x3906, 0x10},
	{0x3901, 0x0a},
	{0x3731, 0x12},
	{0x3620, 0x52},
	{0x371b, 0x20},
	{0x471c, 0x50},
	{0x3a13, 0x43}, // pre-gain = 1.047x
	{0x3a18, 0x00}, // gain ceiling
	{0x3a19, 0xf8}, // gain ceiling = 15.5x
	{0x3635, 0x13},
	{0x3636, 0x03},
	{0x3634, 0x40},
	{0x3622, 0x01},

	{0x3c01, 0x34}, // Band auto, bit[7]
	{0x3c04, 0x28}, // threshold low sum
	{0x3c05, 0x98}, // threshold high sum
	{0x3c06, 0x00}, // light meter 1 threshold[15:8]
	{0x3c07, 0x07}, // light meter 1 threshold[7:0]
	{0x3c08, 0x00}, // light meter 2 threshold[15:8]
	{0x3c09, 0x1c}, // light meter 2 threshold[7:0]
	{0x3c0a, 0x9c}, // sample number[15:8]
	{0x3c0b, 0x40}, // sample number[7:0]

	{0x3800, 0x00}, // HS
	{0x3801, 0x00}, // HS
	{0x3802, 0x00}, // VS
	{0x3803, 0x08}, // VS
	{0x3804, 0x0a}, // HW (HE)
	{0x3805, 0x3f}, // HW (HE)
	{0x3806, 0x07}, // VH (VE)
	{0x3807, 0x9f}, // VH (VE)
	{0x3808, 0x01}, // DVPHO
	{0x3809, 0x40}, // DVPHO
	{0x380a, 0x00}, // DVPVO
	{0x380b, 0xF0}, // DVPVO
	{0x380c, 0x07}, // HTS
	{0x380d, 0x58}, // HTS
	{0x380e, 0x01}, // VTS
	{0x380f, 0xf0}, // VTS
	{0x3810, 0x00}, // HTS
	{0x3811, 0x10}, // HTS
	{0x3812, 0x00}, // VTS
	{0x3813, 0x08}, // VTS
	{0x3814, 0x44}, // timing X inc
	{0x3815, 0x44}, // timing Y inc
	{0x3820, 0x40}, // flip
	{0x3821, 0x00}, // mirror

	{0x3618, 0x00},
	{0x3612, 0x29},
	{0x3709, 0x52},
	{0x370c, 0x03},
	{0x3a02, 0x02}, // 60Hz max exposure
	{0x3a03, 0xe0}, // 60Hz max exposure
	{0x3a14, 0x02}, // 50Hz max exposure
	{0x3a15, 0xe0}, // 50Hz max exposure
	{0x3824, 0x04}, // PCLK manual divider
	{0x5000, 0xa7}, // Lenc on, raw gamma on, BPC on, WPC on, CIP on
	{0x5001, 0xA3}, // SDE on, scale on, UV average off, color matrix on, AWB on
	// {0x501d, 0x40}, // enable manual offset of contrast
	{0X501F, 0x01},
	{0x3a0f, 0x30}, // stable range in high
	{0x3a10, 0x28}, // stable range in low
	{0x3a1b, 0x30}, // stable range out high
	{0x3a1e, 0x26}, // stable range out low
	{0x3a11, 0x60}, // fast zone high
	{0x3a1f, 0x14}, // fast zone low
	{0x5800, 0x23},
	{0x5801, 0x14},
	{0x5802, 0x0f},
	{0x5803, 0x0f},
	{0x5804, 0x12},
	{0x5805, 0x26},
	{0x5806, 0x0c},
	{0x5807, 0x08},
	{0x5808, 0x05},
	{0x5809, 0x05},
	{0x580a, 0x08},
	{0x580b, 0x0d},
	{0x580c, 0x08},
	{0x580d, 0x03},
	{0x580e, 0x00},
	{0x580f, 0x00},
	{0x5810, 0x03},
	{0x5811, 0x09},
	{0x5812, 0x07},
	{0x5813, 0x03},
	{0x5814, 0x00},
	{0x5815, 0x01},
	{0x5816, 0x03},
	{0x5817, 0x08},
	{0x5818, 0x0d},
	{0x5819, 0x08},
	{0x581a, 0x05},
	{0x581b, 0x06},
	{0x581c, 0x08},
	{0x581d, 0x0e},
	{0x581e, 0x29},
	{0x581f, 0x17},
	{0x5820, 0x11},
	{0x5821, 0x11},
	{0x5822, 0x15},
	{0x5823, 0x28},
	{0x5824, 0x46},
	{0x5825, 0x26},
	{0x5826, 0x08},
	{0x5827, 0x26},
	{0x5828, 0x64},
	{0x5829, 0x26},
	{0x582a, 0x24},
	{0x582b, 0x22},
	{0x582c, 0x24},
	{0x582d, 0x24},
	{0x582e, 0x06},
	{0x582f, 0x22},
	{0x5830, 0x40},
	{0x5831, 0x42},
	{0x5832, 0x24},
	{0x5833, 0x26},
	{0x5834, 0x24},
	{0x5835, 0x22},
	{0x5836, 0x22},
	{0x5837, 0x26},
	{0x5838, 0x44},
	{0x5839, 0x24},
	{0x583a, 0x26},
	{0x583b, 0x28},
	{0x583c, 0x42},
	{0x583d, 0xce}, // lenc BR offset
	{0x5180, 0xff}, // AWB B block
	{0x5181, 0xf2}, // AWB control
	{0x5182, 0x00}, // [7:4] max local counter, [3:0] max fast counter
	{0x5183, 0x14}, // AWB advanced
	{0x5184, 0x25},
	{0x5185, 0x24},
	{0x5186, 0x09},
	{0x5187, 0x09},
	{0x5188, 0x09},
	{0x5189, 0x75},
	{0x518a, 0x54},
	{0x518b, 0xe0},
	{0x518c, 0xb2},
	{0x518d, 0x42},
	{0x518e, 0x3d},
	{0x518f, 0x56},
	{0x5190, 0x46},
	// {0x5191, 0xf8}, // AWB top limit
	// {0x5192, 0x04}, // AWB bottom limit
	// {0x5193, 0x70}, // red limit
	// {0x5194, 0xf0}, // green limit
	// {0x5195, 0xf0}, // blue limit
	{0x5196, 0x03}, // AWB control
	{0x5197, 0x01}, // local limit
	{0x5198, 0x04},
	{0x5199, 0x12},
	{0x519a, 0x04},
	{0x519b, 0x00},
	{0x519c, 0x06},
	{0x519d, 0x82},
	{0x519e, 0x38}, // AWB control
	{0x5580, 0x06}, // saturation on, bit[1]
	{0x5583, 0x40},
	{0x5584, 0x10},
	{0x5589, 0x10},
	{0x558a, 0x00},
	{0x558b, 0xf8},
	{0x5480, 0x01}, // Gamma bias plus on, bit[0]
	{0x5481, 0x08},
	{0x5482, 0x14},
	{0x5483, 0x28},
	{0x5484, 0x51},
	{0x5485, 0x65},
	{0x5486, 0x71},
	{0x5487, 0x7d},
	{0x5488, 0x87},
	{0x5489, 0x91},
	{0x548a, 0x9a},
	{0x548b, 0xaa},
	{0x548c, 0xb8},
	{0x548d, 0xcd},
	{0x548e, 0xdd},
	{0x548f, 0xea},
	{0x5490, 0x1d},
	{0x5300, 0x08}, // CIP sharpen MT threshold 1
	{0x5301, 0x30}, // CIP sharpen MT threshold 2
	{0x5302, 0x10}, // CIP sharpen MT offset 1
	{0x5303, 0x00}, // CIP sharpen MT offset 2
	{0x5304, 0x08}, // CIP DNS threshold 1
	{0x5305, 0x30}, // CIP DNS threshold 2
	{0x5306, 0x08}, // CIP DNS offset 1
	{0x5307, 0x16}, // CIP DNS offset 2
	{0x5309, 0x08}, // CIP sharpen TH threshold 1
	{0x530a, 0x30}, // CIP sharpen TH threshold 2
	{0x530b, 0x04}, // CIP sharpen TH offset 1
	{0x530c, 0x06}, // CIP sharpen TH offset 2
	{0x5381, 0x1e}, // CMX1 for Y
	{0x5382, 0x5b}, // CMX2 for Y
	{0x5383, 0x08}, // CMX3 for Y
	{0x5384, 0x0a}, // CMX4 for U
	{0x5385, 0x7e}, // CMX5 for U
	{0x5386, 0x88}, // CMX6 for U
	{0x5387, 0x7c}, // CMX7 for V
	{0x5388, 0x6c}, // CMX8 for V
	{0x5389, 0x10}, // CMX9 for V
	{0x538a, 0x01}, // sign[9]
	{0x538b, 0x98}, // sign[8:1]
	{0x3708, 0x64},
	{0x4001, 0x02}, // BLC start from line 2
	{0x4004, 0x02}, // BLC line number
	{0x4005, 0x1a}, // BLC always update
	{0x4300, 0x61},
	{0x4740, 0X21}, //VSYNC active HIGH
	{0x3017, 0x7f}, // Vsync, HREF, PCLK, D[9:6] output enable
	{0x3018, 0xf0}, // D[5:0] output enable
	{0x3008, 0x02},
	{0x0000, 0x00}
};

static void hal_delay(uint32_t delay)
{
    usleep(delay * 1000);
}

static void ov5640_wr_reg(uint16_t reg,uint8_t data)
{
    dvp_sccb_send_data(OV5640_ADDR, reg, data);
}

static uint8_t ov5640_rd_reg(uint16_t reg)
{
    return dvp_sccb_receive_data(OV5640_ADDR, reg);
}

static uint8_t ov5640_init(void)
{
    uint16_t i = 0;
    uint16_t reg = 0;
    uint8_t chip_id = 0x00;
    chip_id = ov5640_rd_reg(OV5640_CHIPIDH);
    rt_kprintf("ov5640 chip_id:0x%x\n",chip_id);
    for(i = 0; i<sizeof(ov5640_config) / 4; i++)
        ov5640_wr_reg(ov5640_config[i][0], ov5640_config[i][1]);

    return 0x00;
}

static rt_size_t _read_size;
static int dvp_irq_cb(void *ctx) {
    /* 读取DVP中断状态，如果完成则刷新显示地址的数据，并清除中断标志，否则读取摄像头数据*/
    if (dvp_get_interrupt(DVP_STS_FRAME_FINISH)) {
        dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
        dvp_clear_interrupt(DVP_STS_FRAME_FINISH);
        if(((rt_device_t)ctx)->rx_indicate != NULL){
            ((rt_device_t)ctx)->rx_indicate((rt_device_t)ctx, _read_size);
        }
    } else {
        dvp_start_convert();
        dvp_clear_interrupt(DVP_STS_FRAME_START);
    }
    return 0;
}

rt_err_t  drv_ov5640_init(rt_device_t dev){
    
    dvp_init(16);  /* DVP初始化，设置sccb的寄存器长度为16bit */
    dvp_set_xclk_rate(12000000);  /* 设置输入时钟为12000000*/
    dvp_enable_burst();  /* 使能突发传输模式 */
    dvp_set_output_enable(0, 1);  /* 关闭AI输出模式，使能显示模式 */
    dvp_set_output_enable(1, 1);
    dvp_set_image_format(DVP_CFG_RGB_FORMAT);  /* 设置输出格式为RGB */
    dvp_set_image_size(320, 240);  /* 设置输出像素大小为320*240 */
    ov5640_init();  /* 摄像头初始化 */
    return RT_EOK;
}

rt_err_t  drv_ov5640_open(rt_device_t dev, rt_uint16_t oflag){
      /* 设置图像开始和结束中断状态，使能或禁用 */
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 0);
    dvp_disable_auto();  /* 禁用自动接收图像模式 */

    /* DVP 中断配置：中断优先级，中断回调，使能DVP中断 */
    rt_kprintf("DVP interrupt config\n");
    plic_set_priority(IRQN_DVP_INTERRUPT, 1);
    plic_irq_register(IRQN_DVP_INTERRUPT, dvp_irq_cb, (void *)dev);
    plic_irq_enable(IRQN_DVP_INTERRUPT);
    return RT_EOK;
}

rt_err_t  drv_ov5640_close(rt_device_t dev){
    
    plic_irq_disable(IRQN_DVP_INTERRUPT);
    return RT_EOK;
}

rt_size_t drv_ov5640_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size){
    uint32_t dis_buf, ai_buf;
    dis_buf = (uint32_t) buffer;
    ai_buf = dis_buf + (320 * 240 * 2);
    dvp_set_display_addr((uint32_t)buffer);
    dvp_set_ai_addr((uint32_t)ai_buf, (uint32_t)(ai_buf + 320 * 240),
                    (uint32_t)(ai_buf + 320 * 240 * 2));
	_read_size = size;
    dvp_clear_interrupt(DVP_STS_FRAME_START | DVP_STS_FRAME_FINISH);
    dvp_config_interrupt(DVP_CFG_START_INT_ENABLE | DVP_CFG_FINISH_INT_ENABLE, 1);
        
    return RT_EOK;
}

rt_size_t drv_ov5640_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size){
    /* no need */
    return RT_EOK;
}

rt_err_t  drv_ov5640_control(rt_device_t dev, int cmd, void *args){
    /* no need */
    return RT_EOK;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops drv_ov5640_ops =
{
    drv_ov5640_init,
    drv_ov5640_open,
    drv_ov5640_close,
    drv_ov5640_read,
    drv_ov5640_write,
    drv_ov5640_control
};
#endif


int rt_hw_ov5640_init(void)
{
    rt_err_t ret = RT_EOK;

    _ov5640.type        = RT_Device_Class_Sensor;
    _ov5640.rx_indicate = RT_NULL;
    _ov5640.tx_complete = RT_NULL;

#ifdef RT_USING_DEVICE_OPS
    _ov5640.ops        = &drv_ov5640_ops;
#else
    _ov5640.init    = drv_ov5640_init;
    _ov5640.open    = drv_ov5640_open;
    _ov5640.close   = drv_ov5640_close;
    _ov5640.read    = drv_ov5640_read;
    _ov5640.write   = drv_ov5640_write;
    _ov5640.control = drv_ov5640_control;
#endif
    _ov5640.user_data = RT_NULL;

    ret = rt_device_register(&_ov5640, "ov5640", RT_DEVICE_FLAG_INT_RX);
    return ret;
}
INIT_DEVICE_EXPORT(rt_hw_ov5640_init);

#endif
#endif