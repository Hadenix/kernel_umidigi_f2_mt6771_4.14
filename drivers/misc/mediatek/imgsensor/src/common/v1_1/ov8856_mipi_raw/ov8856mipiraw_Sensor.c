/*
 * Copyright (C) 2017 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */
/*****************************************************************************
 *
 * Filename:
 * ---------
 *	 OV8856_mipiraw_sensor.c
 *
 * Project:
 * --------
 *	 ALPS MT6758
 *
 * Description:
 * ------------
 *	 Source code of Sensor driver
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/atomic.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <mt-plat/mtk_boot.h>
#include "kd_camera_typedef.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_feature.h"

#include "ov8856mipiraw_Sensor.h"

#define PFX "OV8856"
#define OV8856_OTP
#ifdef OV8856_OTP
static struct otp_struct *otp_ptr;
#endif
#define LOG_INF(fmt, args...)   pr_info(PFX "[%s] " fmt, __func__, ##args)


static DEFINE_SPINLOCK(imgsensor_drv_lock);

static struct imgsensor_info_struct imgsensor_info = {
	.sensor_id = OV8856_SENSOR_ID,		/*record sensor id defined in Kd_imgsensor.h*/
	.checksum_value = 0xca71b5d,//0xb1893b4f,
	.pre = {
		.pclk = 144000000,
		.linelength  = 1932,
		.framelength = 2482,
		.startx = 0,
		.starty = 0,
		.grabwindow_width  = 1632,
		.grabwindow_height = 1224,

		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.max_framerate = 300,
		.mipi_pixel_rate = 288000000,
	},
	.cap = {
		.pclk = 144000000,
		.linelength  = 1932,
		.framelength = 2482,
		.startx = 0,
		.starty = 0,
		.grabwindow_width  = 3264,
		.grabwindow_height = 2448,
		.mipi_data_lp2hs_settle_dc = 85,
		.max_framerate = 300,
		.mipi_pixel_rate = 288000000,
	},
	.cap1 = {							/*capture for 15fps*/
		.pclk = 144000000,
		.linelength  = 1932,
		.framelength = 4964,
		.startx = 0,
		.starty = 0,
		.grabwindow_width  = 3264,
		.grabwindow_height = 2448,
		.mipi_data_lp2hs_settle_dc = 85,
		.max_framerate = 150,
		.mipi_pixel_rate = 288000000,
	},
	.normal_video = { /* cap*/
		.pclk = 144000000,
		.linelength  = 1932,
		.framelength = 2482,
		.startx = 0,
		.starty = 0,
		.grabwindow_width  = 3264,
		.grabwindow_height = 2448,
		.mipi_data_lp2hs_settle_dc = 85,
		.max_framerate = 300,
		.mipi_pixel_rate = 288000000,
	},
	.hs_video = {
		.pclk = 144000000,
		.linelength  = 1932,
		.framelength = 620,
		.startx = 0,
		.starty = 0,
		.grabwindow_width  = 640,
		.grabwindow_height = 480,
		.mipi_data_lp2hs_settle_dc = 30,
		.max_framerate = 1200,
		.mipi_pixel_rate = 288000000,
	},
	.slim_video = {/*pre*/
		.pclk = 144000000,
		.linelength  = 1932,
		.framelength = 2482,
		.startx = 0,
		.starty = 0,
		.grabwindow_width  = 1632,
		.grabwindow_height = 1224,

		.mipi_data_lp2hs_settle_dc = 85,
		/*	 following for GetDefaultFramerateByScenario()	*/
		.max_framerate = 300,
		.mipi_pixel_rate = 288000000,
	},
	.margin = 6,			/*sensor framelength & shutter margin*/
	.min_shutter = 6,		/*min shutter*/
	.max_frame_length = 0x7ff9,/*max framelength by sensor register's limitation*/
	.ae_shut_delay_frame = 0,
	.ae_sensor_gain_delay_frame = 0,
	.ae_ispGain_delay_frame = 2,/*isp gain delay frame for AE cycle*/
	.ihdr_support = 0,	  /*1, support; 0,not support*/
	.ihdr_le_firstline = 0,  /*1,le first ; 0, se first*/
	.sensor_mode_num = 5,	  /*support sensor mode num ,don't support Slow motion*/
	.cap_delay_frame = 3,		/*enter capture delay frame num*/
	.pre_delay_frame = 3,		/*enter preview delay frame num*/
	.video_delay_frame = 3,		/*enter video delay frame num*/
	.hs_video_delay_frame = 3,	/*enter high speed video  delay frame num*/
	.slim_video_delay_frame = 3,/*enter slim video delay frame num*/
	.isp_driving_current = ISP_DRIVING_8MA, /*mclk driving current*/
	.sensor_interface_type = SENSOR_INTERFACE_TYPE_MIPI,/*Sensor_interface_type*/
	.mipi_sensor_type = MIPI_OPHY_NCSI2, /*0,MIPI_OPHY_NCSI2;  1,MIPI_OPHY_CSI2*/
	.mipi_settle_delay_mode = MIPI_SETTLEDELAY_MANUAL,
	.sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_B,/*sensor output first pixel color*/
	.mclk = 24,/*mclk value, suggest 24 or 26 for 24Mhz or 26Mhz*/
	.mipi_lane_num = SENSOR_MIPI_4_LANE,/*mipi lane num*/
	.i2c_addr_table = {0x6c, 0xff},
};


static struct imgsensor_struct imgsensor = {
	.mirror = IMAGE_HV_MIRROR,				/*mirrorflip information*/
	.sensor_mode = IMGSENSOR_MODE_INIT,
	.shutter = 0x4C00,					/*current shutter*/
	.gain = 0x200,						/*current gain*/
	.dummy_pixel = 0,					/*current dummypixel*/
	.dummy_line = 0,					/*current dummyline*/
	.current_fps = 30,  /*full size current fps : 24fps for PIP, 30fps for Normal or ZSD*/
	.autoflicker_en = KAL_FALSE,
	.test_pattern = KAL_FALSE,
	.current_scenario_id = MSDK_SCENARIO_ID_CAMERA_PREVIEW,/*current scenario id*/
	.ihdr_en = 0, /*sensor need support LE, SE with HDR feature*/
	.i2c_write_id = 0x6c,/*record current sensor's i2c write id*/
};


/* Sensor output window information*/
static struct SENSOR_WINSIZE_INFO_STRUCT imgsensor_winsize_info[5] = {
	/* Preview */
	{ 3296, 2480, 0, 12, 3296, 2456, 1648, 1228, 8,	2, 1632, 1224, 0, 0, 1632, 1224},
	/* capture */
	{ 3296, 2480, 0, 12, 3296, 2456, 3296, 2456, 16, 4, 3264, 2448, 0, 0, 3264, 2448},
	/* video*/
	{ 3296, 2480, 0, 12, 3296, 2456, 3296, 2456, 16, 4, 3264, 2448, 0, 0, 3264, 2448},
	/*hight speed video */
	{ 3296, 2480, 336, 272, 2624, 1936, 656, 484, 8, 2, 640, 480, 0, 0,  640,  480},
	/* slim video  */
	{ 3296, 2480, 0, 12, 3296, 2456, 1648, 1228, 8,	2, 1632, 1224, 0, 0, 1632, 1224}
};

#define SET_STREAMING_TEST 0
#define MULTI_WRITE 1

#if MULTI_WRITE
#define I2C_BUFFER_LEN 225
#else
#define I2C_BUFFER_LEN 3
#endif

static kal_uint16 ov8856_table_write_cmos_sensor(kal_uint16 *para, kal_uint32 len)
{
	char puSendCmd[I2C_BUFFER_LEN];
	kal_uint32 tosend, IDX;
	kal_uint16 addr = 0, addr_last = 0, data;

	tosend = 0;
	IDX = 0;
	while (len > IDX) {
		addr = para[IDX];

		{
			puSendCmd[tosend++] = (char)(addr >> 8);
			puSendCmd[tosend++] = (char)(addr & 0xFF);
			data = para[IDX + 1];
			puSendCmd[tosend++] = (char)(data & 0xFF);
			IDX += 2;
			addr_last = addr;

		}
#if MULTI_WRITE
		/* Write when remain buffer size is less than 3 bytes or reach end of data */
		if ((I2C_BUFFER_LEN - tosend) < 3 || IDX == len || addr != addr_last) {
			iBurstWriteReg_multi(puSendCmd, tosend, imgsensor.i2c_write_id, 3,
				 imgsensor_info.i2c_speed);
			tosend = 0;
		}
#else
		iWriteRegI2C(puSendCmd, 3, imgsensor.i2c_write_id);
		tosend = 0;

#endif
	}
	return 0;
}

static kal_uint16 read_cmos_sensor(kal_uint32 addr)
{
	kal_uint16 get_byte = 0;
	char pu_send_cmd[2] = {(char)(addr >> 8), (char)(addr & 0xFF) };

	iReadRegI2C(pu_send_cmd, 2, (u8 *)&get_byte, 1, imgsensor.i2c_write_id);
	return get_byte;
}

static void write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
	char pu_send_cmd[3] = {(char)(addr >> 8), (char)(addr & 0xFF), (char)(para & 0xFF)};

	iWriteRegI2C(pu_send_cmd, 3, imgsensor.i2c_write_id);
}

static void set_dummy(void)
{
	LOG_INF("dummyline = %d, dummypixels = %d\n", imgsensor.dummy_line, imgsensor.dummy_pixel);
	write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
	write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
	write_cmos_sensor(0x380c, imgsensor.line_length >> 8);
	write_cmos_sensor(0x380d, imgsensor.line_length & 0xFF);
}	/*	set_dummy  */


static void set_max_framerate(UINT16 framerate, kal_bool min_framelength_en)
{
	kal_uint32 frame_length;

	LOG_INF("framerate = %d, min framelength should enable?\n", framerate);

	frame_length = imgsensor.pclk / framerate * 10 / imgsensor.line_length;
	spin_lock(&imgsensor_drv_lock);
	imgsensor.frame_length =
		(frame_length > imgsensor.min_frame_length)
		? frame_length : imgsensor.min_frame_length;
	imgsensor.dummy_line = imgsensor.frame_length - imgsensor.min_frame_length;
	if (imgsensor.frame_length > imgsensor_info.max_frame_length) {
		imgsensor.frame_length = imgsensor_info.max_frame_length;
		imgsensor.dummy_line = imgsensor.frame_length - imgsensor.min_frame_length;
	}
	if (min_framelength_en)
		imgsensor.min_frame_length = imgsensor.frame_length;
	spin_unlock(&imgsensor_drv_lock);
	set_dummy();
}	/*	set_max_framerate  */




static void set_shutter_frame_length(kal_uint16 shutter, kal_uint16 frame_length)
{
	unsigned long flags;
	kal_uint16 realtime_fps = 0;
	kal_int32 dummy_line = 0;

	spin_lock_irqsave(&imgsensor_drv_lock, flags);
	imgsensor.shutter = shutter;
	spin_unlock_irqrestore(&imgsensor_drv_lock, flags);
	spin_lock(&imgsensor_drv_lock);
	/*Change frame time*/
	dummy_line = frame_length - imgsensor.frame_length;
	imgsensor.frame_length = imgsensor.frame_length + dummy_line;
	imgsensor.min_frame_length = imgsensor.frame_length;

	if (shutter > imgsensor.min_frame_length - imgsensor_info.margin)
		imgsensor.frame_length = shutter + imgsensor_info.margin;
	else
		imgsensor.frame_length = imgsensor.min_frame_length;
	if (imgsensor.frame_length > imgsensor_info.max_frame_length)
		imgsensor.frame_length = imgsensor_info.max_frame_length;
	spin_unlock(&imgsensor_drv_lock);
	shutter = (shutter < imgsensor_info.min_shutter) ? imgsensor_info.min_shutter : shutter;
	if (shutter > (imgsensor_info.max_frame_length - imgsensor_info.margin))
		shutter = (imgsensor_info.max_frame_length - imgsensor_info.margin);

	if (imgsensor.autoflicker_en) {
		realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
		if (realtime_fps >= 297 && realtime_fps <= 305)
			set_max_framerate(296, 0);
		else if (realtime_fps >= 147 && realtime_fps <= 150)
			set_max_framerate(146, 0);
		else {
			write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
			write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
		}
	} else {
		write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
		write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
	}

	/* Update Shutter*/
	write_cmos_sensor(0x3502, (shutter << 4) & 0xFF);
	write_cmos_sensor(0x3501, (shutter >> 4) & 0xFF);
	write_cmos_sensor(0x3500, (shutter >> 12) & 0x0F);

	LOG_INF("Add for N3D! shutterlzl =%d, framelength =%d\n", shutter, imgsensor.frame_length);

}

static void set_shutter(kal_uint16 shutter)
{
	unsigned long flags;
	kal_uint16 realtime_fps = 0;

	spin_lock_irqsave(&imgsensor_drv_lock, flags);
	imgsensor.shutter = shutter;
	spin_unlock_irqrestore(&imgsensor_drv_lock, flags);

	/* 0x3500, 0x3501, 0x3502 will increase VBLANK to get exposure larger than frame exposure */
	/* OV Recommend Solution*/
	/* if shutter bigger than frame_length, should extend frame length first*/
	spin_lock(&imgsensor_drv_lock);
	if (shutter > imgsensor.min_frame_length - imgsensor_info.margin)
		imgsensor.frame_length = shutter + imgsensor_info.margin;
	else
		imgsensor.frame_length = imgsensor.min_frame_length;
	if (imgsensor.frame_length > imgsensor_info.max_frame_length)
		imgsensor.frame_length = imgsensor_info.max_frame_length;
	spin_unlock(&imgsensor_drv_lock);
	shutter = (shutter < imgsensor_info.min_shutter) ? imgsensor_info.min_shutter : shutter;
	if (shutter > (imgsensor_info.max_frame_length - imgsensor_info.margin))
		shutter = (imgsensor_info.max_frame_length - imgsensor_info.margin);

	if (imgsensor.autoflicker_en) {
		realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
		if (realtime_fps >= 297 && realtime_fps <= 305)
			set_max_framerate(296, 0);
		else if (realtime_fps >= 147 && realtime_fps <= 150)
			set_max_framerate(146, 0);
		else {
			write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
			write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
		}
	} else {
		write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
		write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
	}

	/* Update Shutter*/
	write_cmos_sensor(0x3502, (shutter << 4) & 0xFF);
	write_cmos_sensor(0x3501, (shutter >> 4) & 0xFF);
	write_cmos_sensor(0x3500, (shutter >> 12) & 0x0F);
	LOG_INF("Exit! shutter =%d, framelength =%d\n", shutter, imgsensor.frame_length);

}

static kal_uint16 set_gain(kal_uint16 gain)
{
	kal_uint16 reg_gain;

	if (gain < BASEGAIN || gain > 15 * BASEGAIN) {
		LOG_INF("Error gain setting");

		if (gain < BASEGAIN)
			gain = BASEGAIN;
		else if (gain > 15 * BASEGAIN)
			gain = 15 * BASEGAIN;
	}

	reg_gain = gain*2;
	spin_lock(&imgsensor_drv_lock);
	imgsensor.gain = reg_gain;
	spin_unlock(&imgsensor_drv_lock);
	LOG_INF("gain = %d , reg_gain = 0x%x\n ", gain, reg_gain);

	write_cmos_sensor(0x3508, (reg_gain>>8));
	write_cmos_sensor(0x3509, (reg_gain&0xFF));
	return gain;
}	/*	set_gain  */

static void ihdr_write_shutter_gain(kal_uint16 le, kal_uint16 se, kal_uint16 gain)
{
	LOG_INF("le:0x%x, se:0x%x, gain:0x%x\n", le, se, gain);
	if (imgsensor.ihdr_en) {
		spin_lock(&imgsensor_drv_lock);
			if (le > imgsensor.min_frame_length - imgsensor_info.margin)
				imgsensor.frame_length = le + imgsensor_info.margin;
			else
				imgsensor.frame_length = imgsensor.min_frame_length;
			if (imgsensor.frame_length > imgsensor_info.max_frame_length)
				imgsensor.frame_length = imgsensor_info.max_frame_length;
			spin_unlock(&imgsensor_drv_lock);
			if (le < imgsensor_info.min_shutter)
				le = imgsensor_info.min_shutter;
			if (se < imgsensor_info.min_shutter)
				se = imgsensor_info.min_shutter;
		/* Extend frame length first*/
		write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
		write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);
		write_cmos_sensor(0x3502, (le << 4) & 0xFF);
		write_cmos_sensor(0x3501, (le >> 4) & 0xFF);
		write_cmos_sensor(0x3500, (le >> 12) & 0x0F);
		write_cmos_sensor(0x3512, (se << 4) & 0xFF);
		write_cmos_sensor(0x3511, (se >> 4) & 0xFF);
		write_cmos_sensor(0x3510, (se >> 12) & 0x0F);
		set_gain(gain);
	}
}



static void set_mirror_flip(kal_uint8 image_mirror)
{
	LOG_INF("image_mirror = %d\n", image_mirror);

	switch (image_mirror) {
	case IMAGE_NORMAL:
			write_cmos_sensor(0x3820, ((read_cmos_sensor(0x3820) & 0xB9) | 0x00));
			write_cmos_sensor(0x3821, ((read_cmos_sensor(0x3821) & 0xF9) | 0x06));
			write_cmos_sensor(0x502e, ((read_cmos_sensor(0x502e) & 0xFC) | 0x03));
			write_cmos_sensor(0x5001, ((read_cmos_sensor(0x5001) & 0xFB) | 0x00));
			write_cmos_sensor(0x5004, ((read_cmos_sensor(0x5004) & 0xFB) | 0x04));
			write_cmos_sensor(0x376b, 0x30);

			break;
	case IMAGE_H_MIRROR:
			write_cmos_sensor(0x3820, ((read_cmos_sensor(0x3820) & 0xB9) | 0x00));
			write_cmos_sensor(0x3821, ((read_cmos_sensor(0x3821) & 0xF9) | 0x00));
			write_cmos_sensor(0x502e, ((read_cmos_sensor(0x502e) & 0xFC) | 0x03));
			write_cmos_sensor(0x5001, ((read_cmos_sensor(0x5001) & 0xFB) | 0x00));
			write_cmos_sensor(0x5004, ((read_cmos_sensor(0x5004) & 0xFB) | 0x00));
			write_cmos_sensor(0x376b, 0x30);
			break;
	case IMAGE_V_MIRROR:
			write_cmos_sensor(0x3820, ((read_cmos_sensor(0x3820) & 0xB9) | 0x46));
			write_cmos_sensor(0x3821, ((read_cmos_sensor(0x3821) & 0xF9) | 0x06));
			write_cmos_sensor(0x502e, ((read_cmos_sensor(0x502e) & 0xFC) | 0x00));
			write_cmos_sensor(0x5001, ((read_cmos_sensor(0x5001) & 0xFB) | 0x04));
			write_cmos_sensor(0x5004, ((read_cmos_sensor(0x5004) & 0xFB) | 0x04));
			write_cmos_sensor(0x376b, 0x36);
			break;
	case IMAGE_HV_MIRROR:
			write_cmos_sensor(0x3820, ((read_cmos_sensor(0x3820) & 0xB9) | 0x46));
			write_cmos_sensor(0x3821, ((read_cmos_sensor(0x3821) & 0xF9) | 0x00));
			write_cmos_sensor(0x502e, ((read_cmos_sensor(0x502e) & 0xFC) | 0x00));
			write_cmos_sensor(0x5001, ((read_cmos_sensor(0x5001) & 0xFB) | 0x04));
			write_cmos_sensor(0x5004, ((read_cmos_sensor(0x5004) & 0xFB) | 0x00));
			write_cmos_sensor(0x376b, 0x36);
			break;
	default:
			LOG_INF("Error image_mirror setting\n");
	}

}

static void night_mode(kal_bool enable)
{
/*No Need to implement this function*/
}	/*	night_mode	*/


kal_uint16 addr_data_pair_init_ov8856[] = {
	0x0103, 0x01,
	0x0302, 0x3c,
	0x0303, 0x01,
	0x031e, 0x0c,
	0x3000, 0x00,
	0x300e, 0x00,
	0x3010, 0x00,
	0x3015, 0x84,
	0x3018, 0x72,
	0x3021, 0x23,
	0x3033, 0x24,
	0x3500, 0x00,
	0x3501, 0x4c,
	0x3502, 0xe0,
	0x3503, 0x08,
	0x3505, 0x83,
	0x3508, 0x01,
	0x3509, 0x80,
	0x350c, 0x00,
	0x350d, 0x80,
	0x350e, 0x04,
	0x350f, 0x00,
	0x3510, 0x00,
	0x3511, 0x02,
	0x3512, 0x00,
	0x3600, 0x72,
	0x3601, 0x40,
	0x3602, 0x30,
	0x3610, 0xc5,
	0x3611, 0x58,
	0x3612, 0x5c,
	0x3613, 0xca,
	0x3614, 0x60,//for R2A
	0x3628, 0xff,
	0x3629, 0xff,
	0x362a, 0xff,
	0x3633, 0x10,
	0x3634, 0x10,
	0x3635, 0x10,
	0x3636, 0x10,
	0x3663, 0x08,
	0x3669, 0x34,
	0x366e, 0x08,
	0x3706, 0x86,
	0x370b, 0x7e,
	0x3714, 0x27,
	0x3730, 0x12,
	0x3733, 0x10,
	0x3764, 0x00,
	0x3765, 0x00,
	0x3769, 0x62,
	0x376a, 0x2a,
	0x376b, 0x36,
	0x3780, 0x00,
	0x3781, 0x24,
	0x3782, 0x00,
	0x3783, 0x23,
	0x3798, 0x2f,
	0x37a1, 0x60,
	0x37a8, 0x6a,
	0x37ab, 0x3f,
	0x37c2, 0x14,
	0x37c3, 0xf1,
	0x37c9, 0x80,
	0x37cb, 0x16,
	0x37cc, 0x16,
	0x37cd, 0x16,
	0x37ce, 0x16,
	0x3800, 0x00,
	0x3801, 0x00,
	0x3802, 0x00,
	0x3803, 0x0c,
	0x3804, 0x0c,
	0x3805, 0xdf,
	0x3806, 0x09,
	0x3807, 0xa3,
	0x3808, 0x06,
	0x3809, 0x60,
	0x380a, 0x04,
	0x380b, 0xc8,
	0x380c, 0x07,
	0x380d, 0x8c,
	0x380e, 0x09,
	0x380f, 0xb2,
	0x3810, 0x00,
	0x3811, 0x08,
	0x3812, 0x00,
	0x3813, 0x02,
	0x3814, 0x03,
	0x3815, 0x01,
	0x3816, 0x00,
	0x3817, 0x00,
	0x3818, 0x00,
	0x3819, 0x00,
	0x3820, 0x46,
	0x3821, 0x00,
	0x382a, 0x03,
	0x382b, 0x01,
	0x3830, 0x06,
	0x3836, 0x02,
	0x3837, 0x10,
	0x3862, 0x04,
	0x3863, 0x08,
	0x3cc0, 0x33,
	0x3d85, 0x17,
	0x3d8c, 0x73,
	0x3d8d, 0xde,
	0x4001, 0xe0,
	0x4003, 0x40,
	0x4008, 0x00,
	0x4009, 0x05,
	0x400a, 0x00,
	0x400b, 0x84,
	0x400f, 0x80,
	0x4010, 0xf0,
	0x4011, 0xff,
	0x4012, 0x02,
	0x4013, 0x01,
	0x4014, 0x01,
	0x4015, 0x01,
	0x4042, 0x00,
	0x4043, 0x80,
	0x4044, 0x00,
	0x4045, 0x80,
	0x4046, 0x00,
	0x4047, 0x80,
	0x4048, 0x00,
	0x4049, 0x80,
	0x4041, 0x03,
	0x404c, 0x20,
	0x404d, 0x00,
	0x404e, 0x20,
	0x4203, 0x80,
	0x4307, 0x30,
	0x4317, 0x00,
	0x4503, 0x08,
	0x4601, 0x80,
	0x4800, 0x44,
	0x4816, 0x53,
	0x481b, 0x58,
	0x481f, 0x27,
	0x4837, 0x16,
	0x483c, 0x0f,
	0x484b, 0x05,
	0x5000, 0x77,
	0x5001, 0x0a,
	0x5004, 0x00,
	0x502e, 0x00,
	0x5030, 0x41,
	0x5795, 0x00,
	0x5796, 0x10,
	0x5797, 0x10,
	0x5798, 0x73,
	0x5799, 0x73,
	0x579a, 0x00,
	0x579b, 0x28,
	0x579c, 0x00,
	0x579d, 0x16,
	0x579e, 0x06,
	0x579f, 0x20,
	0x57a0, 0x04,
	0x57a1, 0xa0,
	0x5780, 0x14,
	0x5781, 0x0f,
	0x5782, 0x44,
	0x5783, 0x02,
	0x5784, 0x01,
	0x5785, 0x01,
	0x5786, 0x00,
	0x5787, 0x04,
	0x5788, 0x02,
	0x5789, 0x0f,
	0x578a, 0xfd,
	0x578b, 0xf5,
	0x578c, 0xf5,
	0x578d, 0x03,
	0x578e, 0x08,
	0x578f, 0x0c,
	0x5790, 0x08,
	0x5791, 0x04,
	0x5792, 0x00,
	0x5793, 0x52,
	0x5794, 0xa3,
	0x59f8, 0x3d,
	0x5a08, 0x02,
	0x5b00, 0x02,
	0x5b01, 0x10,
	0x5b02, 0x03,
	0x5b03, 0xcf,
	0x5b05, 0x6c,
	0x5e00, 0x00,
};

static void sensor_init(void)
{
	LOG_INF("v3 E\n");
#if 1	/* MULTI_WRITE */
	ov8856_table_write_cmos_sensor(addr_data_pair_init_ov8856,
			   sizeof(addr_data_pair_init_ov8856) / sizeof(kal_uint16));
#endif

#if SET_STREAMING_TEST
		write_cmos_sensor(0x0100, 0x01);
#endif
}	/*	sensor_init  */

kal_uint16 addr_data_pair_preview_ov8856[] = {
	0x0302, 0x3c,
	0x0303, 0x01,
	0x031e, 0x0c,
	0x3000, 0x00,
	0x300e, 0x00,
	0x3010, 0x00,
	0x3015, 0x84,
	0x3018, 0x72,
	0x3021, 0x23,
	0x3033, 0x24,
	0x3500, 0x00,
	0x3501, 0x9a,
	0x3502, 0x20,
	0x3503, 0x08,
	0x3505, 0x83,
	0x3508, 0x01,
	0x3509, 0x80,
	0x350c, 0x00,
	0x350d, 0x80,
	0x350e, 0x04,
	0x350f, 0x00,
	0x3510, 0x00,
	0x3511, 0x02,
	0x3512, 0x00,
	0x3600, 0x72,
	0x3601, 0x40,
	0x3602, 0x30,
	0x3610, 0xc5,
	0x3611, 0x58,
	0x3612, 0x5c,
	0x3613, 0xca,
	0x3614, 0x60,//for R2A
	0x3628, 0xff,
	0x3629, 0xff,
	0x362a, 0xff,
	0x3633, 0x10,
	0x3634, 0x10,
	0x3635, 0x10,
	0x3636, 0x10,
	0x3663, 0x08,
	0x3669, 0x34,
	0x366e, 0x08,
	0x3706, 0x86,
	0x370b, 0x7e,
	0x3714, 0x27,
	0x3730, 0x12,
	0x3733, 0x10,
	0x3764, 0x00,
	0x3765, 0x00,
	0x3769, 0x62,
	0x376a, 0x2a,
	0x376b, 0x36,
	0x3780, 0x00,
	0x3781, 0x24,
	0x3782, 0x00,
	0x3783, 0x23,
	0x3798, 0x2f,
	0x37a1, 0x60,
	0x37a8, 0x6a,
	0x37ab, 0x3f,
	0x37c2, 0x14,
	0x37c3, 0xf1,
	0x37c9, 0x80,
	0x37cb, 0x16,
	0x37cc, 0x16,
	0x37cd, 0x16,
	0x37ce, 0x16,
	0x3800, 0x00,
	0x3801, 0x00,
	0x3802, 0x00,
	0x3803, 0x0c,
	0x3804, 0x0c,
	0x3805, 0xdf,
	0x3806, 0x09,
	0x3807, 0xa3,
	0x3808, 0x06,
	0x3809, 0x60,
	0x380a, 0x04,
	0x380b, 0xc8,
	0x380c, 0x07,
	0x380d, 0x8c,
	0x380e, 0x09,
	0x380f, 0xb2,
	0x3810, 0x00,
	0x3811, 0x08,
	0x3812, 0x00,
	0x3813, 0x02,
	0x3814, 0x03,
	0x3815, 0x01,
	0x3816, 0x00,
	0x3817, 0x00,
	0x3818, 0x00,
	0x3819, 0x00,
	0x3820, 0xd6,
	0x3821, 0x61,
	0x382a, 0x03,
	0x382b, 0x01,
	0x3830, 0x06,
	0x3836, 0x02,
	0x3837, 0x10,
	0x3862, 0x04,
	0x3863, 0x08,
	0x3cc0, 0x33,
	0x3d85, 0x17,
	0x3d8c, 0x73,
	0x3d8d, 0xde,
	0x4001, 0xe0,
	0x4003, 0x40,
	0x4008, 0x00,
	0x4009, 0x05,
	0x400a, 0x00,
	0x400b, 0x84,
	0x400f, 0x80,
	0x4010, 0xf0,
	0x4011, 0xff,
	0x4012, 0x02,
	0x4013, 0x01,
	0x4014, 0x01,
	0x4015, 0x01,
	0x4042, 0x00,
	0x4043, 0x80,
	0x4044, 0x00,
	0x4045, 0x80,
	0x4046, 0x00,
	0x4047, 0x80,
	0x4048, 0x00,
	0x4049, 0x80,
	0x4041, 0x03,
	0x404c, 0x20,
	0x404d, 0x00,
	0x404e, 0x20,
	0x4203, 0x80,
	0x4307, 0x30,
	0x4317, 0x00,
	0x4503, 0x08,
	0x4601, 0x80,
	0x4800, 0x44,
	0x4816, 0x53,
	0x481b, 0x58,
	0x481f, 0x27,
	0x4837, 0x16,
	0x483c, 0x0f,
	0x484b, 0x05,
	0x5000, 0x77,
	0x5001, 0x0a,
	0x5004, 0x00,
	0x502e, 0x00,
	0x5030, 0x41,
	0x5795, 0x00,
	0x5796, 0x10,
	0x5797, 0x10,
	0x5798, 0x73,
	0x5799, 0x73,
	0x579a, 0x00,
	0x579b, 0x28,
	0x579c, 0x00,
	0x579d, 0x16,
	0x579e, 0x06,
	0x579f, 0x20,
	0x57a0, 0x04,
	0x57a1, 0xa0,
	0x5780, 0x14,
	0x5781, 0x0f,
	0x5782, 0x44,
	0x5783, 0x02,
	0x5784, 0x01,
	0x5785, 0x01,
	0x5786, 0x00,
	0x5787, 0x04,
	0x5788, 0x02,
	0x5789, 0x0f,
	0x578a, 0xfd,
	0x578b, 0xf5,
	0x578c, 0xf5,
	0x578d, 0x03,
	0x578e, 0x08,
	0x578f, 0x0c,
	0x5790, 0x08,
	0x5791, 0x04,
	0x5792, 0x00,
	0x5793, 0x52,
	0x5794, 0xa3,
	0x59f8, 0x3d,
	0x5a08, 0x02,
	0x5b00, 0x02,
	0x5b01, 0x10,
	0x5b02, 0x03,
	0x5b03, 0xcf,
	0x5b05, 0x6c,
	0x5e00, 0x00,
};
static void preview_setting(void)
{
	LOG_INF("E\n");
#if SET_STREAMING_TEST
		write_cmos_sensor(0x0100, 0x00);
#endif
	ov8856_table_write_cmos_sensor(addr_data_pair_preview_ov8856,
				   sizeof(addr_data_pair_preview_ov8856) / sizeof(kal_uint16));

#if SET_STREAMING_TEST
		write_cmos_sensor(0x0100, 0x01);
#endif
}	/*	preview_setting  */

kal_uint16 addr_data_pair_capture_30fps_ov8856[] = {
	0x0302, 0x3c,
	0x0303, 0x01,
	0x031e, 0x0c,
	0x3000, 0x00,
	0x300e, 0x00,
	0x3010, 0x00,
	0x3015, 0x84,
	0x3018, 0x72,
	0x3021, 0x23,
	0x3033, 0x24,
	0x3500, 0x00,
	0x3501, 0x9a,
	0x3502, 0x20,
	0x3503, 0x08,
	0x3505, 0x83,
	0x3508, 0x01,
	0x3509, 0x80,
	0x350c, 0x00,
	0x350d, 0x80,
	0x350e, 0x04,
	0x350f, 0x00,
	0x3510, 0x00,
	0x3511, 0x02,
	0x3512, 0x00,
	0x3600, 0x72,
	0x3601, 0x40,
	0x3602, 0x30,
	0x3610, 0xc5,
	0x3611, 0x58,
	0x3612, 0x5c,
	0x3613, 0xca,
	0x3614, 0x60,//for R2A
	0x3628, 0xff,
	0x3629, 0xff,
	0x362a, 0xff,
	0x3633, 0x10,
	0x3634, 0x10,
	0x3635, 0x10,
	0x3636, 0x10,
	0x3663, 0x08,
	0x3669, 0x34,
	0x366e, 0x10,
	0x3706, 0x86,
	0x370b, 0x7e,
	0x3714, 0x23,
	0x3730, 0x12,
	0x3733, 0x10,
	0x3764, 0x00,
	0x3765, 0x00,
	0x3769, 0x62,
	0x376a, 0x2a,
	0x376b, 0x36,
	0x3780, 0x00,
	0x3781, 0x24,
	0x3782, 0x00,
	0x3783, 0x23,
	0x3798, 0x2f,
	0x37a1, 0x60,
	0x37a8, 0x6a,
	0x37ab, 0x3f,
	0x37c2, 0x04,
	0x37c3, 0xf1,
	0x37c9, 0x80,
	0x37cb, 0x16,
	0x37cc, 0x16,
	0x37cd, 0x16,
	0x37ce, 0x16,
	0x3800, 0x00,
	0x3801, 0x00,
	0x3802, 0x00,
	0x3803, 0x0c,
	0x3804, 0x0c,
	0x3805, 0xdf,
	0x3806, 0x09,
	0x3807, 0xa3,
	0x3808, 0x0c,
	0x3809, 0xc0,
	0x380a, 0x09,
	0x380b, 0x90,
	0x380c, 0x07,
	0x380d, 0x8c,
	0x380e, 0x09,
	0x380f, 0xb2,
	0x3810, 0x00,
	0x3811, 0x10,
	0x3812, 0x00,
	0x3813, 0x04,
	0x3814, 0x01,
	0x3815, 0x01,
	0x3816, 0x00,
	0x3817, 0x00,
	0x3818, 0x00,
	0x3819, 0x00,
	0x3820, 0x46,
	0x3821, 0x00,
	0x382a, 0x01,
	0x382b, 0x01,
	0x3830, 0x06,
	0x3836, 0x02,
	0x3837, 0x10,
	0x3862, 0x04,
	0x3863, 0x08,
	0x3cc0, 0x33,
	0x3d85, 0x17,
	0x3d8c, 0x73,
	0x3d8d, 0xde,
	0x4001, 0xe0,
	0x4003, 0x40,
	0x4008, 0x00,
	0x4009, 0x0b,
	0x400a, 0x00,
	0x400b, 0x84,
	0x400f, 0x80,
	0x4010, 0xf0,
	0x4011, 0xff,
	0x4012, 0x02,
	0x4013, 0x01,
	0x4014, 0x01,
	0x4015, 0x01,
	0x4042, 0x00,
	0x4043, 0x80,
	0x4044, 0x00,
	0x4045, 0x80,
	0x4046, 0x00,
	0x4047, 0x80,
	0x4048, 0x00,
	0x4049, 0x80,
	0x4041, 0x03,
	0x404c, 0x20,
	0x404d, 0x00,
	0x404e, 0x20,
	0x4203, 0x80,
	0x4307, 0x30,
	0x4317, 0x00,
	0x4503, 0x08,
	0x4601, 0x80,
	0x4800, 0x44,
	0x4816, 0x53,
	0x481b, 0x58,
	0x481f, 0x27,
	0x4837, 0x16,
	0x483c, 0x0f,
	0x484b, 0x05,
	0x5000, 0x77,
	0x5001, 0x0a,
	0x5004, 0x00,
	0x502e, 0x00,
	0x5030, 0x41,
	0x5795, 0x02,
	0x5796, 0x20,
	0x5797, 0x20,
	0x5798, 0xd5,
	0x5799, 0xd5,
	0x579a, 0x00,
	0x579b, 0x50,
	0x579c, 0x00,
	0x579d, 0x2c,
	0x579e, 0x0c,
	0x579f, 0x40,
	0x57a0, 0x09,
	0x57a1, 0x40,
	0x5780, 0x14,
	0x5781, 0x0f,
	0x5782, 0x44,
	0x5783, 0x02,
	0x5784, 0x01,
	0x5785, 0x01,
	0x5786, 0x00,
	0x5787, 0x04,
	0x5788, 0x02,
	0x5789, 0x0f,
	0x578a, 0xfd,
	0x578b, 0xf5,
	0x578c, 0xf5,
	0x578d, 0x03,
	0x578e, 0x08,
	0x578f, 0x0c,
	0x5790, 0x08,
	0x5791, 0x04,
	0x5792, 0x00,
	0x5793, 0x52,
	0x5794, 0xa3,
	0x59f8, 0x3d,
	0x5a08, 0x02,
	0x5b00, 0x02,
	0x5b01, 0x10,
	0x5b02, 0x03,
	0x5b03, 0xcf,
	0x5b05, 0x6c,
	0x5e00, 0x00,
};

kal_uint16 addr_data_pair_capture_15fps_ov8856[] = {
	0x0302, 0x3c,
	0x0303, 0x01,
	0x031e, 0x0c,
	0x3000, 0x00,
	0x300e, 0x00,
	0x3010, 0x00,
	0x3015, 0x84,
	0x3018, 0x72,
	0x3021, 0x23,
	0x3033, 0x24,
	0x3500, 0x00,
	0x3501, 0x9a,
	0x3502, 0x20,
	0x3503, 0x08,
	0x3505, 0x83,
	0x3508, 0x01,
	0x3509, 0x80,
	0x350c, 0x00,
	0x350d, 0x80,
	0x350e, 0x04,
	0x350f, 0x00,
	0x3510, 0x00,
	0x3511, 0x02,
	0x3512, 0x00,
	0x3600, 0x72,
	0x3601, 0x40,
	0x3602, 0x30,
	0x3610, 0xc5,
	0x3611, 0x58,
	0x3612, 0x5c,
	0x3613, 0xca,
	0x3614, 0x60,//for R2A
	0x3628, 0xff,
	0x3629, 0xff,
	0x362a, 0xff,
	0x3633, 0x10,
	0x3634, 0x10,
	0x3635, 0x10,
	0x3636, 0x10,
	0x3663, 0x08,
	0x3669, 0x34,
	0x366e, 0x10,
	0x3706, 0x86,
	0x370b, 0x7e,
	0x3714, 0x23,
	0x3730, 0x12,
	0x3733, 0x10,
	0x3764, 0x00,
	0x3765, 0x00,
	0x3769, 0x62,
	0x376a, 0x2a,
	0x376b, 0x36,
	0x3780, 0x00,
	0x3781, 0x24,
	0x3782, 0x00,
	0x3783, 0x23,
	0x3798, 0x2f,
	0x37a1, 0x60,
	0x37a8, 0x6a,
	0x37ab, 0x3f,
	0x37c2, 0x04,
	0x37c3, 0xf1,
	0x37c9, 0x80,
	0x37cb, 0x16,
	0x37cc, 0x16,
	0x37cd, 0x16,
	0x37ce, 0x16,
	0x3800, 0x00,
	0x3801, 0x00,
	0x3802, 0x00,
	0x3803, 0x0c,
	0x3804, 0x0c,
	0x3805, 0xdf,
	0x3806, 0x09,
	0x3807, 0xa3,
	0x3808, 0x0c,
	0x3809, 0xc0,
	0x380a, 0x09,
	0x380b, 0x90,
	0x380c, 0x07,
	0x380d, 0x8c,
	0x380e, 0x13,
	0x380f, 0x64,
	0x3810, 0x00,
	0x3811, 0x10,
	0x3812, 0x00,
	0x3813, 0x04,
	0x3814, 0x01,
	0x3815, 0x01,
	0x3816, 0x00,
	0x3817, 0x00,
	0x3818, 0x00,
	0x3819, 0x00,
	0x3820, 0x46,
	0x3821, 0x00,
	0x382a, 0x01,
	0x382b, 0x01,
	0x3830, 0x06,
	0x3836, 0x02,
	0x3837, 0x10,
	0x3862, 0x04,
	0x3863, 0x08,
	0x3cc0, 0x33,
	0x3d85, 0x17,
	0x3d8c, 0x73,
	0x3d8d, 0xde,
	0x4001, 0xe0,
	0x4003, 0x40,
	0x4008, 0x00,
	0x4009, 0x0b,
	0x400a, 0x00,
	0x400b, 0x84,
	0x400f, 0x80,
	0x4010, 0xf0,
	0x4011, 0xff,
	0x4012, 0x02,
	0x4013, 0x01,
	0x4014, 0x01,
	0x4015, 0x01,
	0x4042, 0x00,
	0x4043, 0x80,
	0x4044, 0x00,
	0x4045, 0x80,
	0x4046, 0x00,
	0x4047, 0x80,
	0x4048, 0x00,
	0x4049, 0x80,
	0x4041, 0x03,
	0x404c, 0x20,
	0x404d, 0x00,
	0x404e, 0x20,
	0x4203, 0x80,
	0x4307, 0x30,
	0x4317, 0x00,
	0x4503, 0x08,
	0x4601, 0x80,
	0x4800, 0x44,
	0x4816, 0x53,
	0x481b, 0x58,
	0x481f, 0x27,
	0x4837, 0x16,
	0x483c, 0x0f,
	0x484b, 0x05,
	0x5000, 0x77,
	0x5001, 0x0a,
	0x5004, 0x00,
	0x502e, 0x00,
	0x5030, 0x41,
	0x5795, 0x02,
	0x5796, 0x20,
	0x5797, 0x20,
	0x5798, 0xd5,
	0x5799, 0xd5,
	0x579a, 0x00,
	0x579b, 0x50,
	0x579c, 0x00,
	0x579d, 0x2c,
	0x579e, 0x0c,
	0x579f, 0x40,
	0x57a0, 0x09,
	0x57a1, 0x40,
	0x5780, 0x14,
	0x5781, 0x0f,
	0x5782, 0x44,
	0x5783, 0x02,
	0x5784, 0x01,
	0x5785, 0x01,
	0x5786, 0x00,
	0x5787, 0x04,
	0x5788, 0x02,
	0x5789, 0x0f,
	0x578a, 0xfd,
	0x578b, 0xf5,
	0x578c, 0xf5,
	0x578d, 0x03,
	0x578e, 0x08,
	0x578f, 0x0c,
	0x5790, 0x08,
	0x5791, 0x04,
	0x5792, 0x00,
	0x5793, 0x52,
	0x5794, 0xa3,
	0x59f8, 0x3d,
	0x5a08, 0x02,
	0x5b00, 0x02,
	0x5b01, 0x10,
	0x5b02, 0x03,
	0x5b03, 0xcf,
	0x5b05, 0x6c,
	0x5e00, 0x00,
};

static void capture_setting(kal_uint16 currefps)
{
	LOG_INF("E! currefps:%d\n", currefps);
#if SET_STREAMING_TEST
		write_cmos_sensor(0x0100, 0x00);
#endif
	if (currefps == 150) {
		ov8856_table_write_cmos_sensor(addr_data_pair_capture_15fps_ov8856,
					       sizeof(addr_data_pair_capture_15fps_ov8856) /
					       sizeof(kal_uint16));
	} else {
		ov8856_table_write_cmos_sensor(addr_data_pair_capture_30fps_ov8856,
					       sizeof(addr_data_pair_capture_30fps_ov8856) /
					       sizeof(kal_uint16));

	}
#if SET_STREAMING_TEST
		write_cmos_sensor(0x0100, 0x01);
#endif
}



kal_uint16 addr_data_pair_vga_setting_120fps_ov8856[] = {
	0x3501, 0x25,
	0x3502, 0xc0,
	0x366e, 0x08,
	0x3714, 0x29,
	0x37c2, 0x34,
	0x3800, 0x01,
	0x3801, 0x50,
	0x3802, 0x01,
	0x3803, 0x10,
	0x3804, 0x0b,
	0x3805, 0x8f,
	0x3806, 0x08,
	0x3807, 0x9f,
	0x3808, 0x02,
	0x3809, 0x80,
	0x380a, 0x01,
	0x380b, 0xe0,
	0x380c, 0x07,
	0x380d, 0x8c,
	0x380e, 0x02,
	0x380f, 0x6c,
	0x3810, 0x00,
	0x3811, 0x07,
	0x3812, 0x00,
	0x3813, 0x02,
	0x3814, 0x03,
	0x3820, 0xd6,
	0x3821, 0x61,
	0x382a, 0x07,
	0x4009, 0x05,
	0x4601, 0x40,
	0x5795, 0x00,
	0x5796, 0x10,
	0x5797, 0x10,
	0x5798, 0x73,
	0x5799, 0x73,
	0x579a, 0x00,
	0x579b, 0x00,
	0x579c, 0x00,
	0x579d, 0x00,
	0x579e, 0x05,
	0x579f, 0xa0,
	0x57a0, 0x03,
	0x57a1, 0x20,
	0x366d, 0x11,
	0x5003, 0xc0,
	0x5006, 0x02,
	0x5007, 0x90,
	0x5e10, 0x7c,
};

static void vga_setting_120fps(void)
{
#if SET_STREAMING_TEST
		write_cmos_sensor(0x0100, 0x01);
#endif

	ov8856_table_write_cmos_sensor(addr_data_pair_vga_setting_120fps_ov8856,
				   sizeof(addr_data_pair_vga_setting_120fps_ov8856) /
				   sizeof(kal_uint16));

#if SET_STREAMING_TEST
		write_cmos_sensor(0x0100, 0x01);
#endif
}

static void hs_video_setting(void)
{
	LOG_INF("E\n");
	vga_setting_120fps();
}

static void slim_video_setting(void)
{
	LOG_INF("E\n");
	preview_setting();
}


static kal_uint32 get_imgsensor_id(UINT32 *sensor_id)
{
	kal_uint8 i = 0;
	kal_uint8 retry = 2;

	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		spin_lock(&imgsensor_drv_lock);
		imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
		spin_unlock(&imgsensor_drv_lock);
		do {
			*sensor_id = ((read_cmos_sensor(0x300B) << 8) | read_cmos_sensor(0x300C));
			if (*sensor_id == imgsensor_info.sensor_id) {
				LOG_INF("i2c id:0x%x,sensor id:0x%x,ver=0x%x<0xB1=r2a,0xB0=r1a>\n",
					imgsensor.i2c_write_id,
					*sensor_id, read_cmos_sensor(0x302A));
				return ERROR_NONE;
			}
			LOG_INF("Read sensor id fail,write_id:0x%x, id: 0x%x\n",
			imgsensor.i2c_write_id, *sensor_id);
			retry--;
		} while (retry > 0);
		i++;
		retry = 2;
	}
	if (*sensor_id != imgsensor_info.sensor_id) {
		/* if Sensor ID is not correct, Must set *sensor_id to 0xFFFFFFFF */
		*sensor_id = 0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}
	return ERROR_NONE;
}

#ifdef OV8856_OTP

struct otp_struct {
	int flag;	/* bit[7]: info, bit[6]:wb, bit[5]:vcm, bit[4]:lenc */
	int module_integrator_id;
	int lens_id;
	int sensor_id;
	int production_year;
	int production_month;
	int production_day;
	int rg_ratio;
	int bg_ratio;
	int rg_Golden;
	int bg_Golden;
	int awbc[12];
	int lenc[240];
	int checksum;
};

/* return value: */
/* bit[7]: 0 no otp info, 1 valid otp info */
/* bit[6]: 0 no otp wb, 1 valib otp wb */
/* bit[5]: 0 no otp vcm, 1 valid otp vcm */
/* bit[4]: 0 no otp lenc/invalid otp lenc, 1 valid otp lenc */
int read_otp(struct otp_struct *otp_ptr)
{
	int otp_flag, addr, temp, i;
	/* set 0x5001[3] to ?0? */
	int temp1;	int checksum2 = 0;

	int r_awb_data, gr_awb_data, gb_awb_data, b_awb_data;

	temp1 = read_cmos_sensor(0x5001);
	write_cmos_sensor(0x5001, (0x00 & 0x08) | (temp1 & (~0x08)));
	/* read OTP into buffer */
	write_cmos_sensor(0x3d84, 0xC0);
	write_cmos_sensor(0x3d88, 0x70);	/* OTP start address */
	write_cmos_sensor(0x3d89, 0x10);
	write_cmos_sensor(0x3d8A, 0x72);	/* OTP end address */
	write_cmos_sensor(0x3d8B, 0x0e);
	write_cmos_sensor(0x3d81, 0x01);	/* load otp into buffer */
	mdelay(10);
	/* OTP base information and WB calibration data */
	otp_flag = read_cmos_sensor(0x7010);
	LOG_INF("ov8856 otp_flag is %d\n", otp_flag);
	addr = 0;
	if ((otp_flag & 0xc0) == 0x40)
		addr = 0x7011;	/* base address of info group 1 */
	else if ((otp_flag & 0x30) == 0x10)
		addr = 0x701B;	/* base address of info group 2 */

	if (addr != 0) {
		(*otp_ptr).flag = 0xC0;	/* valid info and AWB in OTP */
		(*otp_ptr).module_integrator_id = read_cmos_sensor(addr);
		(*otp_ptr).sensor_id = read_cmos_sensor(addr + 1);
		temp = read_cmos_sensor(addr + 6);

		r_awb_data = (read_cmos_sensor(addr + 2) << 2) + ((temp >> 6) & 0x03);
		gr_awb_data = (read_cmos_sensor(addr + 3) << 2) + ((temp >> 4) & 0x03);
		gb_awb_data = (read_cmos_sensor(addr + 4) << 2) + ((temp >> 2) & 0x03);
		b_awb_data = (read_cmos_sensor(addr + 5) << 2) + (temp & 0x03);
		LOG_INF("r_awb_data = %d,gr_awb_data = %d,b_awb_data = %d,gb_awb_data = %d\n",
			r_awb_data, gr_awb_data, b_awb_data, gb_awb_data);
		(*otp_ptr).rg_ratio =
			((r_awb_data * 1023/((gr_awb_data + gb_awb_data)/2)) * 10 + 5)/10;

		(*otp_ptr).bg_ratio =
			((b_awb_data * 1023/((gr_awb_data + gb_awb_data)/2)) * 10 + 5)/10;

		LOG_INF("rg_ratio: %d, bg_ratio: %d in line 1633\n",
			(*otp_ptr).rg_ratio, (*otp_ptr).bg_ratio);

		(*otp_ptr).rg_Golden = 600;

		(*otp_ptr).bg_Golden = 700;

		LOG_INF("rg_Golden: %d, bg_Golden: %d\n",
			(*otp_ptr).rg_Golden, (*otp_ptr).bg_Golden);

		for (i = 0; i < 10; i++) {
			(*otp_ptr).awbc[i] = read_cmos_sensor(addr + i);
			checksum2 += (*otp_ptr).awbc[i];
		}

		if (addr == 0x7011)
			checksum2 = checksum2 + read_cmos_sensor(0x720B) + read_cmos_sensor(0x720C);
		else
			checksum2 = checksum2 + read_cmos_sensor(0x720D) + read_cmos_sensor(0x720E);

		checksum2 = (checksum2) % 255 + 1;

		if (addr == 0x7011)
			(*otp_ptr).checksum = read_cmos_sensor(0x7025);
		else
			(*otp_ptr).checksum = read_cmos_sensor(0x7026);

		if ((*otp_ptr).checksum == checksum2) {
			(*otp_ptr).flag |= 0x20;
			if (addr == 0x7011) {
				ov8856_af_inf = read_cmos_sensor(0x7018);
				ov8856_af_mac = read_cmos_sensor(0x7019);
				ov8856_af_lsb = read_cmos_sensor(0x701A);
			} else {
				ov8856_af_inf = read_cmos_sensor(0x7022);
				ov8856_af_mac = read_cmos_sensor(0x7023);
				ov8856_af_lsb = read_cmos_sensor(0x7024);
			}
		}

		LOG_INF("awb,af,group id checksum = %d,checksum2 = %d\n",
		(*otp_ptr).checksum, checksum2);
		LOG_INF("AFInf = %d,AFMac = %d,AFLsb = %d\n",
		ov8856_af_inf, ov8856_af_mac, ov8856_af_lsb);
	} else {
		(*otp_ptr).flag = 0x00;	/* not info and AWB in OTP */
		(*otp_ptr).module_integrator_id = 0;
		(*otp_ptr).lens_id = 0;
		(*otp_ptr).production_year = 0;
		(*otp_ptr).production_month = 0;
		(*otp_ptr).production_day = 0;
		(*otp_ptr).rg_ratio = 0;
		(*otp_ptr).bg_ratio = 0;
	}
	/* OTP Lenc Calibration */
	otp_flag = read_cmos_sensor(0x7028);
	addr = 0;
	checksum2 = 0;

	if ((otp_flag & 0xc0) == 0x40)
		addr = 0x7029;	/* base address of Lenc Calibration group 1 */
	else if ((otp_flag & 0x30) == 0x10)
		addr = 0x711A;	/* base address of Lenc Calibration group 2 */

	if (addr != 0) {
		for (i = 0; i < 240; i++) {
			(*otp_ptr).lenc[i] = read_cmos_sensor(addr + i);
			checksum2 += (*otp_ptr).lenc[i];
		}
		checksum2 = (checksum2) % 255 + 1;
		(*otp_ptr).checksum = read_cmos_sensor(addr + 240);

		LOG_INF("lsc checksum = %d,checksum2 = %d\n", (*otp_ptr).checksum, checksum2);

		if ((*otp_ptr).checksum == checksum2)
			(*otp_ptr).flag |= 0x10;

	} else {
		for (i = 0; i < 240; i++)
			(*otp_ptr).lenc[i] = 0;
	}
	for (i = 0x7010; i <= 0x720F; i++)
		write_cmos_sensor(i, 0);
	/* clear OTP buffer, recommended use continuous write to accelarate */

	/* set 0x5001[3] to ?1? */
	temp1 = read_cmos_sensor(0x5001);
	write_cmos_sensor(0x5001, (0x08 & 0x08) | (temp1 & (~0x08)));
	return (*otp_ptr).flag;
}

/* return value: */
/* bit[7]: 0 no otp info, 1 valid otp info */
/* bit[6]: 0 no otp wb, 1 valib otp wb */
/* bit[5]: 0 no otp vcm, 1 valid otp vcm */
/* bit[4]: 0 no otp lenc, 1 valid otp lenc */

int apply_otp(struct otp_struct *otp_ptr)
{
	int RG_Ratio_Typical = (*otp_ptr).rg_Golden, BG_Ratio_Typical = (*otp_ptr).bg_Golden;
	int rg, bg, R_gain, G_gain, B_gain, Base_gain, temp, i;

	LOG_INF("flag = %d\n", (*otp_ptr).flag);
	/* apply OTP WB Calibration */
	if ((*otp_ptr).flag & 0x20) {
		rg = (*otp_ptr).rg_ratio;
		bg = (*otp_ptr).bg_ratio;
		/* calculate G gain */
		R_gain = (RG_Ratio_Typical * 1000) / rg;
		B_gain = (BG_Ratio_Typical * 1000) / bg;
		G_gain = 1000;
		if (R_gain < 1000 || B_gain < 1000) {
			if (R_gain < B_gain)
				Base_gain = R_gain;
			else
				Base_gain = B_gain;
		} else {
			Base_gain = G_gain;
		}
		R_gain = 0x400 * R_gain / (Base_gain);
		B_gain = 0x400 * B_gain / (Base_gain);
		G_gain = 0x400 * G_gain / (Base_gain);

		LOG_INF("R_gain: 0x%x, B_gain: 0x%x, G_gain: 0x%x\n",
			R_gain, B_gain, G_gain);
		/* update sensor WB gain */
		if (R_gain > 0x400) {
			write_cmos_sensor(0x5019, R_gain >> 8);
			write_cmos_sensor(0x501a, R_gain & 0x00ff);
		}
		if (G_gain > 0x400) {
			write_cmos_sensor(0x501b, G_gain >> 8);
			write_cmos_sensor(0x501c, G_gain & 0x00ff);
		}
		if (B_gain > 0x400) {
			write_cmos_sensor(0x501d, B_gain >> 8);
			write_cmos_sensor(0x501e, B_gain & 0x00ff);
		}
	}
	/* apply OTP Lenc Calibration */
	if ((*otp_ptr).flag & 0x10) {
		temp = read_cmos_sensor(0x5000);
		temp = 0x20 | temp;
		write_cmos_sensor(0x5000, temp);
		for (i = 0; i < 240; i++)
			write_cmos_sensor(0x5900 + i, (*otp_ptr).lenc[i]);
		LOG_INF("apply LSC Success\n");
	}
	return (*otp_ptr).flag;
}

#endif

static kal_uint32 open(void)
{
	/*const kal_uint8 i2c_addr[] = {IMGSENSOR_WRITE_ID_1, IMGSENSOR_WRITE_ID_2};*/
	kal_uint8 i = 0;
	kal_uint8 retry = 2;
	kal_uint16 sensor_id = 0;

	LOG_INF("PLATFORM:Vison,MIPI 4LANE\n");
	LOG_INF("read_cmos_sensor(0x302A): 0x%x\n", read_cmos_sensor(0x302A));

	while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		spin_lock(&imgsensor_drv_lock);
		imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
		spin_unlock(&imgsensor_drv_lock);
		do {
			sensor_id = ((read_cmos_sensor(0x300B) << 8) | read_cmos_sensor(0x300C));
			if (sensor_id == imgsensor_info.sensor_id) {
				LOG_INF("i2c write id: 0x%x, sensor id: 0x%x\n",
				imgsensor.i2c_write_id, sensor_id);
				break;
			}
			LOG_INF("Read sensor id fail, write: 0x%x, sensor: 0x%x\n",
			imgsensor.i2c_write_id, sensor_id);
			retry--;
		} while (retry > 0);
		i++;
		if (sensor_id == imgsensor_info.sensor_id)
			break;
		retry = 2;
	}
	if (imgsensor_info.sensor_id != sensor_id)
		return ERROR_SENSOR_CONNECT_FAIL;
	/* initail sequence write in  */
	sensor_init();
#ifdef OV8856_OTP
	write_cmos_sensor(0x100, 0x01);
	mdelay(10);
	LOG_INF("Apply the sensor OTP\n");
	otp_ptr = kzalloc(sizeof(struct otp_struct), GFP_KERNEL);
	read_otp(otp_ptr);
	apply_otp(otp_ptr);
	kfree(otp_ptr);
	write_cmos_sensor(0x100, 0x00);
	mdelay(20);
#endif
	spin_lock(&imgsensor_drv_lock);

	imgsensor.autoflicker_en = KAL_FALSE;
	imgsensor.sensor_mode = IMGSENSOR_MODE_INIT;
	imgsensor.shutter = 0x2D00;
	imgsensor.gain = 0x100;
	imgsensor.pclk = imgsensor_info.pre.pclk;
	imgsensor.frame_length = imgsensor_info.pre.framelength;
	imgsensor.line_length = imgsensor_info.pre.linelength;
	imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	imgsensor.dummy_pixel = 0;
	imgsensor.dummy_line = 0;
	imgsensor.ihdr_en = 0;
	imgsensor.test_pattern = KAL_FALSE;
	imgsensor.current_fps = imgsensor_info.pre.max_framerate;
	spin_unlock(&imgsensor_drv_lock);

	return ERROR_NONE;
}	/*	open  */



static kal_uint32 close(void)
{
	LOG_INF("E\n");

	/*No Need to implement this function*/
	return ERROR_NONE;
}	/*	close  */


static kal_uint32 preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_PREVIEW;
	imgsensor.pclk = imgsensor_info.pre.pclk;
	/*imgsensor.video_mode = KAL_FALSE;*/
	imgsensor.line_length = imgsensor_info.pre.linelength;
	imgsensor.frame_length = imgsensor_info.pre.framelength;
	imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	preview_setting();
	set_mirror_flip(imgsensor.mirror);
	mdelay(10);
	return ERROR_NONE;
}	/*	preview   */


static kal_uint32 capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
						  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_CAPTURE;
	if (imgsensor.current_fps == imgsensor_info.cap1.max_framerate) {/*15fps*/
		imgsensor.pclk = imgsensor_info.cap1.pclk;
		imgsensor.line_length = imgsensor_info.cap1.linelength;
		imgsensor.frame_length = imgsensor_info.cap1.framelength;
		imgsensor.min_frame_length = imgsensor_info.cap1.framelength;
		imgsensor.autoflicker_en = KAL_FALSE;
	} else {
		if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
			LOG_INF("Warning:current_fps %d not support,use cap1's setting:%dfps!\n",
				imgsensor.current_fps, imgsensor_info.cap1.max_framerate/10);
		imgsensor.pclk = imgsensor_info.cap.pclk;
		imgsensor.line_length = imgsensor_info.cap.linelength;
		imgsensor.frame_length = imgsensor_info.cap.framelength;
		imgsensor.min_frame_length = imgsensor_info.cap.framelength;
		imgsensor.autoflicker_en = KAL_FALSE;
	}
	spin_unlock(&imgsensor_drv_lock);

	capture_setting(imgsensor.current_fps);
	mdelay(10);

	if (imgsensor.test_pattern == KAL_TRUE) {
		/*write_cmos_sensor(0x5001,0x00);*/
		write_cmos_sensor(0x5000, (read_cmos_sensor(0x5000)&0xBF)|0x00);
	}
	set_mirror_flip(imgsensor.mirror);
	return ERROR_NONE;
}	/* capture() */
static kal_uint32 normal_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");
	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_VIDEO;
	imgsensor.pclk = imgsensor_info.normal_video.pclk;
	imgsensor.line_length = imgsensor_info.normal_video.linelength;
	imgsensor.frame_length = imgsensor_info.normal_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.normal_video.framelength;
	/*imgsensor.current_fps = 300;*/
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	capture_setting(imgsensor.current_fps);
	set_mirror_flip(imgsensor.mirror);
	mdelay(10);
	return ERROR_NONE;
}	/*	normal_video   */

static kal_uint32 hs_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_HIGH_SPEED_VIDEO;
	imgsensor.pclk = imgsensor_info.hs_video.pclk;
	/*imgsensor.video_mode = KAL_TRUE;*/
	imgsensor.line_length = imgsensor_info.hs_video.linelength;
	imgsensor.frame_length = imgsensor_info.hs_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.hs_video.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	/*imgsensor.current_fps = 300;*/
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	hs_video_setting();
	set_mirror_flip(imgsensor.mirror);
	mdelay(10);

	return ERROR_NONE;
}	/*	hs_video   */

static kal_uint32 slim_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("E\n");

	spin_lock(&imgsensor_drv_lock);
	imgsensor.sensor_mode = IMGSENSOR_MODE_SLIM_VIDEO;
	imgsensor.pclk = imgsensor_info.slim_video.pclk;
	/*imgsensor.video_mode = KAL_TRUE;*/
	imgsensor.line_length = imgsensor_info.slim_video.linelength;
	imgsensor.frame_length = imgsensor_info.slim_video.framelength;
	imgsensor.min_frame_length = imgsensor_info.slim_video.framelength;
	imgsensor.dummy_line = 0;
	imgsensor.dummy_pixel = 0;
	/*imgsensor.current_fps = 300;*/
	imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	slim_video_setting();
	set_mirror_flip(imgsensor.mirror);
	mdelay(10);

	return ERROR_NONE;
}	/*	slim_video	 */



static kal_uint32 get_resolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *sensor_resolution)
{
	LOG_INF("E\n");
	sensor_resolution->SensorFullWidth = imgsensor_info.cap.grabwindow_width;
	sensor_resolution->SensorFullHeight = imgsensor_info.cap.grabwindow_height;
	sensor_resolution->SensorPreviewWidth = imgsensor_info.pre.grabwindow_width;
	sensor_resolution->SensorPreviewHeight = imgsensor_info.pre.grabwindow_height;
	sensor_resolution->SensorVideoWidth = imgsensor_info.normal_video.grabwindow_width;
	sensor_resolution->SensorVideoHeight = imgsensor_info.normal_video.grabwindow_height;
	sensor_resolution->SensorHighSpeedVideoWidth = imgsensor_info.hs_video.grabwindow_width;
	sensor_resolution->SensorHighSpeedVideoHeight = imgsensor_info.hs_video.grabwindow_height;
	sensor_resolution->SensorSlimVideoWidth = imgsensor_info.slim_video.grabwindow_width;
	sensor_resolution->SensorSlimVideoHeight = imgsensor_info.slim_video.grabwindow_height;
	return ERROR_NONE;
}	/*	get_resolution	*/

static kal_uint32 get_info(enum MSDK_SCENARIO_ID_ENUM scenario_id,
					  MSDK_SENSOR_INFO_STRUCT *sensor_info,
					  MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("scenario_id = %d\n", scenario_id);

	sensor_info->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW; /* not use */
	sensor_info->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW; /* inverse with datasheet*/
	sensor_info->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
	sensor_info->SensorInterruptDelayLines = 4; /* not use */
	sensor_info->SensorResetActiveHigh = FALSE; /* not use */
	sensor_info->SensorResetDelayCount = 5; /* not use */
	sensor_info->SensroInterfaceType = imgsensor_info.sensor_interface_type;
	sensor_info->MIPIsensorType = imgsensor_info.mipi_sensor_type;
	sensor_info->SettleDelayMode = imgsensor_info.mipi_settle_delay_mode;
	sensor_info->SensorOutputDataFormat = imgsensor_info.sensor_output_dataformat;
	sensor_info->CaptureDelayFrame = imgsensor_info.cap_delay_frame;
	sensor_info->PreviewDelayFrame = imgsensor_info.pre_delay_frame;
	sensor_info->VideoDelayFrame = imgsensor_info.video_delay_frame;
	sensor_info->HighSpeedVideoDelayFrame = imgsensor_info.hs_video_delay_frame;
	sensor_info->SlimVideoDelayFrame = imgsensor_info.slim_video_delay_frame;
	sensor_info->SensorMasterClockSwitch = 0; /* not use */
	sensor_info->SensorDrivingCurrent = imgsensor_info.isp_driving_current;
	sensor_info->AEShutDelayFrame = imgsensor_info.ae_shut_delay_frame;
	sensor_info->AESensorGainDelayFrame = imgsensor_info.ae_sensor_gain_delay_frame;
	sensor_info->AEISPGainDelayFrame = imgsensor_info.ae_ispGain_delay_frame;
	sensor_info->IHDR_Support = imgsensor_info.ihdr_support;
	sensor_info->IHDR_LE_FirstLine = imgsensor_info.ihdr_le_firstline;
	sensor_info->SensorModeNum = imgsensor_info.sensor_mode_num;
	sensor_info->SensorMIPILaneNumber = imgsensor_info.mipi_lane_num;
	sensor_info->SensorClockFreq = imgsensor_info.mclk;
	sensor_info->SensorClockDividCount = 3; /* not use */
	sensor_info->SensorClockRisingCount = 0;
	sensor_info->SensorClockFallingCount = 2; /* not use */
	sensor_info->SensorPixelClockCount = 3; /* not use */
	sensor_info->SensorDataLatchCount = 2; /* not use */
	sensor_info->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;
	sensor_info->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
	sensor_info->SensorWidthSampling = 0;  /* 0 is default 1x*/
	sensor_info->SensorHightSampling = 0;	/* 0 is default 1x */
	sensor_info->SensorPacketECCOrder = 1;

	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
			sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;
			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
				imgsensor_info.pre.mipi_data_lp2hs_settle_dc;

			break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			sensor_info->SensorGrabStartX = imgsensor_info.cap.startx;
			sensor_info->SensorGrabStartY = imgsensor_info.cap.starty;
			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
				imgsensor_info.cap.mipi_data_lp2hs_settle_dc;

			break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			sensor_info->SensorGrabStartX = imgsensor_info.normal_video.startx;
			sensor_info->SensorGrabStartY = imgsensor_info.normal_video.starty;
			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
				imgsensor_info.normal_video.mipi_data_lp2hs_settle_dc;

			break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			sensor_info->SensorGrabStartX = imgsensor_info.hs_video.startx;
			sensor_info->SensorGrabStartY = imgsensor_info.hs_video.starty;
			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
				imgsensor_info.hs_video.mipi_data_lp2hs_settle_dc;

			break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
			sensor_info->SensorGrabStartX = imgsensor_info.slim_video.startx;
			sensor_info->SensorGrabStartY = imgsensor_info.slim_video.starty;
			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
				imgsensor_info.slim_video.mipi_data_lp2hs_settle_dc;
			break;
	default:
			sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
			sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;
			sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount =
				imgsensor_info.pre.mipi_data_lp2hs_settle_dc;
			break;
	}
	return ERROR_NONE;
}	/*	get_info  */


static kal_uint32 control(enum MSDK_SCENARIO_ID_ENUM scenario_id,
	MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
	MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
	LOG_INF("scenario_id = %d\n", scenario_id);
	spin_lock(&imgsensor_drv_lock);
	imgsensor.current_scenario_id = scenario_id;
	spin_unlock(&imgsensor_drv_lock);
	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			preview(image_window, sensor_config_data);
			break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			capture(image_window, sensor_config_data);
			break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			normal_video(image_window, sensor_config_data);
			break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			hs_video(image_window, sensor_config_data);
			break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
			slim_video(image_window, sensor_config_data);
			break;
	default:
			LOG_INF("Error ScenarioId setting");
			preview(image_window, sensor_config_data);
			return ERROR_INVALID_SCENARIO_ID;
	}
	return ERROR_NONE;
}	/* control() */



static kal_uint32 set_video_mode(UINT16 framerate)
{
	LOG_INF("framerate = %d\n ", framerate);
	/* SetVideoMode Function should fix framerate*/
	if (framerate == 0)
		/* Dynamic frame rate*/
		return ERROR_NONE;
	spin_lock(&imgsensor_drv_lock);
	if ((framerate == 300) && (imgsensor.autoflicker_en == KAL_TRUE))
		imgsensor.current_fps = 296;
	else if ((framerate == 150) && (imgsensor.autoflicker_en == KAL_TRUE))
		imgsensor.current_fps = 146;
	else
		imgsensor.current_fps = framerate;
	spin_unlock(&imgsensor_drv_lock);
	set_max_framerate(imgsensor.current_fps, 1);

	return ERROR_NONE;
}

static kal_uint32 set_auto_flicker_mode(kal_bool enable, UINT16 framerate)
{
	LOG_INF("enable = %d, framerate = %d\n", enable, framerate);
	spin_lock(&imgsensor_drv_lock);
	if (enable) /*enable auto flicker	  */
		imgsensor.autoflicker_en = KAL_TRUE;
	else /*Cancel Auto flick*/
		imgsensor.autoflicker_en = KAL_FALSE;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}


static kal_uint32 set_max_framerate_by_scenario(
	enum MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32 framerate)
{
	kal_uint32 frame_length;

	LOG_INF("scenario_id = %d, framerate = %d\n", scenario_id, framerate);
	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			frame_length = imgsensor_info.pre.pclk
			/ framerate * 10 / imgsensor_info.pre.linelength;
			spin_lock(&imgsensor_drv_lock);
			if (frame_length > imgsensor_info.pre.framelength)
				imgsensor.dummy_line = (frame_length
					- imgsensor_info.pre.framelength);
			else
				imgsensor.dummy_line = 0;
			imgsensor.frame_length = imgsensor_info.pre.framelength
				+ imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			if (framerate == 0)
				return ERROR_NONE;
			frame_length = imgsensor_info.normal_video.pclk / framerate * 10;
			frame_length /= imgsensor_info.normal_video.linelength;
			spin_lock(&imgsensor_drv_lock);
			if (frame_length > imgsensor_info.normal_video.framelength)
				imgsensor.dummy_line =
					(frame_length - imgsensor_info.normal_video.framelength);
			else
				imgsensor.dummy_line = 0;
			imgsensor.frame_length = imgsensor_info.normal_video.framelength
				+ imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			frame_length = imgsensor_info.cap.pclk
				/ framerate * 10 / imgsensor_info.cap.linelength;
			spin_lock(&imgsensor_drv_lock);
			if (frame_length > imgsensor_info.cap.framelength)
				imgsensor.dummy_line =
					(frame_length - imgsensor_info.cap.framelength);
			else
				imgsensor.dummy_line = 0;
			imgsensor.frame_length = imgsensor_info.cap.framelength
				+ imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			frame_length = imgsensor_info.hs_video.pclk / framerate * 10;
			frame_length /= imgsensor_info.hs_video.linelength;
			spin_lock(&imgsensor_drv_lock);
			if (frame_length > imgsensor_info.hs_video.framelength)
				imgsensor.dummy_line =
					(frame_length - imgsensor_info.hs_video.framelength);
			else
				imgsensor.dummy_line = 0;
			imgsensor.frame_length =
				imgsensor_info.hs_video.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
			frame_length = imgsensor_info.slim_video.pclk / framerate * 10;
			frame_length /= imgsensor_info.slim_video.linelength;
			spin_lock(&imgsensor_drv_lock);
			if (frame_length > imgsensor_info.slim_video.framelength)
				imgsensor.dummy_line =
					(frame_length - imgsensor_info.slim_video.framelength);
			else
				imgsensor.dummy_line = 0;
			imgsensor.frame_length =
				imgsensor_info.slim_video.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			break;
	default:  /*coding with  preview scenario by default*/
			frame_length = imgsensor_info.pre.pclk
				/ framerate * 10 / imgsensor_info.pre.linelength;
			spin_lock(&imgsensor_drv_lock);
			if (frame_length > imgsensor_info.pre.framelength)
				imgsensor.dummy_line =
					(frame_length - imgsensor_info.pre.framelength);
			else
				imgsensor.dummy_line = 0;
			imgsensor.frame_length =
				imgsensor_info.pre.framelength + imgsensor.dummy_line;
			imgsensor.min_frame_length = imgsensor.frame_length;
			spin_unlock(&imgsensor_drv_lock);
			LOG_INF("error scenario_id = %d, we use preview scenario\n", scenario_id);
			break;
	}
	return ERROR_NONE;
}


static kal_uint32 get_default_framerate_by_scenario(
	enum MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32 *framerate)
{
	LOG_INF("scenario_id = %d\n", scenario_id);
	switch (scenario_id) {
	case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			*framerate = imgsensor_info.pre.max_framerate;
			break;
	case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*framerate = imgsensor_info.normal_video.max_framerate;
			break;
	case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*framerate = imgsensor_info.cap.max_framerate;
			break;
	case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*framerate = imgsensor_info.hs_video.max_framerate;
			break;
	case MSDK_SCENARIO_ID_SLIM_VIDEO:
			*framerate = imgsensor_info.slim_video.max_framerate;
			break;
	default:
			break;
	}
	return ERROR_NONE;
}

static kal_uint32 set_test_pattern_mode(kal_bool enable)
{
	LOG_INF("enable: %d\n", enable);

	if (enable) {
		write_cmos_sensor(0x5000, 0x57);
		write_cmos_sensor(0x5001, 0x02);
		write_cmos_sensor(0x5e00, 0x80);
	} else {
		write_cmos_sensor(0x5000, 0x77);
		write_cmos_sensor(0x5001, 0x0a);
		write_cmos_sensor(0x5e00, 0x00);
	}
	spin_lock(&imgsensor_drv_lock);
	imgsensor.test_pattern = enable;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}

static kal_uint32 streaming_control(kal_bool enable)
{
	LOG_INF("streaming_enable(0=Sw Standby,1=streaming): %d\n", enable);
	if (enable)
		write_cmos_sensor(0x0100, 0X01);
	else
		write_cmos_sensor(0x0100, 0x00);
	return ERROR_NONE;
}

static kal_uint32 feature_control(MSDK_SENSOR_FEATURE_ENUM feature_id,
	UINT8 *feature_para, UINT32 *feature_para_len)
{
	UINT16 *feature_return_para_16 = (UINT16 *) feature_para;
	UINT16 *feature_data_16 = (UINT16 *) feature_para;
	UINT32 *feature_return_para_32 = (UINT32 *) feature_para;
	UINT32 *feature_data_32 = (UINT32 *) feature_para;
	uintptr_t tmp;
	unsigned long long *feature_data = (unsigned long long *) feature_para;
	struct SENSOR_WINSIZE_INFO_STRUCT *wininfo;
	MSDK_SENSOR_REG_INFO_STRUCT *sensor_reg_data = (MSDK_SENSOR_REG_INFO_STRUCT *) feature_para;

	LOG_INF("feature_id = %d\n", feature_id);
	switch (feature_id) {
	case SENSOR_FEATURE_GET_PERIOD:
		*feature_return_para_16++ = imgsensor.line_length;
		*feature_return_para_16 = imgsensor.frame_length;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
		*feature_return_para_32 = imgsensor.pclk;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_SET_ESHUTTER:
		set_shutter(*feature_data);
		break;
	case SENSOR_FEATURE_SET_NIGHTMODE:
		night_mode((BOOL) (*feature_data));
		break;
	case SENSOR_FEATURE_SET_GAIN:
		set_gain((UINT16) *feature_data);
		break;
	case SENSOR_FEATURE_SET_FLASHLIGHT:
		break;
	case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
		break;
	case SENSOR_FEATURE_SET_REGISTER:
		write_cmos_sensor(sensor_reg_data->RegAddr, sensor_reg_data->RegData);
		break;
	case SENSOR_FEATURE_GET_REGISTER:
		sensor_reg_data->RegData = read_cmos_sensor(sensor_reg_data->RegAddr);
		break;
	case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
		*feature_return_para_32 = LENS_DRIVER_ID_DO_NOT_CARE;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_SET_VIDEO_MODE:
		set_video_mode(*feature_data);
		break;
	case SENSOR_FEATURE_CHECK_SENSOR_ID:
		get_imgsensor_id(feature_return_para_32);
		break;
	case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
		set_auto_flicker_mode((BOOL)*feature_data_16, *(feature_data_16+1));
		break;
	case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
		set_max_framerate_by_scenario(
			(enum MSDK_SCENARIO_ID_ENUM)*feature_data,
			*(feature_data+1));
		break;
	case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
		get_default_framerate_by_scenario(
			(enum MSDK_SCENARIO_ID_ENUM)*(feature_data),
			(MUINT32 *)(uintptr_t)(*(feature_data+1)));
		break;
	case SENSOR_FEATURE_SET_TEST_PATTERN:
		set_test_pattern_mode((BOOL)*feature_data);
		break;
	case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE: /*for factory mode auto testing*/
		*feature_return_para_32 = imgsensor_info.checksum_value;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_SET_FRAMERATE:
		LOG_INF("current fps :%d\n", *feature_data_32);
		spin_lock(&imgsensor_drv_lock);
		imgsensor.current_fps = (UINT16)*feature_data_32;
		spin_unlock(&imgsensor_drv_lock);
		break;
	case SENSOR_FEATURE_SET_HDR:
		LOG_INF("ihdr enable :%d\n", *feature_data_32);
		spin_lock(&imgsensor_drv_lock);
		imgsensor.ihdr_en = (UINT8)*feature_data_32;
		spin_unlock(&imgsensor_drv_lock);
		break;
	case SENSOR_FEATURE_GET_CROP_INFO:
		LOG_INF("SENSOR_FEATURE_GET_CROP_INFO scenarioId:%d\n", (UINT32)*feature_data);
		tmp = (uintptr_t)(*(feature_data+1));

		wininfo = (struct SENSOR_WINSIZE_INFO_STRUCT *)tmp;

		switch (*feature_data_32) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[1],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[2],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[3],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[4],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			memcpy((void *)wininfo,
				(void *)&imgsensor_winsize_info[0],
				sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
			break;
		}
		break;
	case SENSOR_FEATURE_SET_IHDR_SHUTTER_GAIN:
		LOG_INF("SENSOR_SET_SENSOR_IHDR LE=%d, SE=%d, Gain=%d\n",
			(UINT16)*feature_data, (UINT16)*(feature_data+1),
			(UINT16)*(feature_data+2));
		ihdr_write_shutter_gain((UINT16)*feature_data,
			(UINT16)*(feature_data+1), (UINT16)*(feature_data+2));
		break;
	case SENSOR_FEATURE_SET_SHUTTER_FRAME_TIME:
		set_shutter_frame_length((UINT16) (*feature_data), (UINT16) (*(feature_data + 1)));
		break;
	case SENSOR_FEATURE_GET_TEMPERATURE_VALUE:
		LOG_INF("This sensor can't support temperature get\n");
		break;
	case SENSOR_FEATURE_SET_STREAMING_SUSPEND:
		LOG_INF("SENSOR_FEATURE_SET_STREAMING_SUSPEND\n");
		streaming_control(KAL_FALSE);
		break;
	case SENSOR_FEATURE_SET_STREAMING_RESUME:
		LOG_INF("SENSOR_FEATURE_SET_STREAMING_RESUME, shutter:%llu\n", *feature_data);
		if (*feature_data != 0)
			set_shutter(*feature_data);
		streaming_control(KAL_TRUE);
		break;
	case SENSOR_FEATURE_GET_PIXEL_RATE:
		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.cap.pclk /
			(imgsensor_info.cap.linelength - 80))*
			imgsensor_info.cap.grabwindow_width;

			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.normal_video.pclk /
			(imgsensor_info.normal_video.linelength - 80))*
			imgsensor_info.normal_video.grabwindow_width;

			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.hs_video.pclk /
			(imgsensor_info.hs_video.linelength - 80))*
			imgsensor_info.hs_video.grabwindow_width;

			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.slim_video.pclk /
			(imgsensor_info.slim_video.linelength - 80))*
			imgsensor_info.slim_video.grabwindow_width;

			break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
			(imgsensor_info.pre.pclk /
			(imgsensor_info.pre.linelength - 80))*
			imgsensor_info.pre.grabwindow_width;
			break;
		}
		break;
	case SENSOR_FEATURE_GET_MIPI_PIXEL_RATE:
		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.cap.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.normal_video.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.hs_video.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.slim_video.mipi_pixel_rate;
			break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1)) =
				imgsensor_info.pre.mipi_pixel_rate;
			break;
		}
		break;
	default:
		break;
	}

	return ERROR_NONE;
}    /*    feature_control()  */


static struct SENSOR_FUNCTION_STRUCT sensor_func = {
	open,
	get_info,
	get_resolution,
	feature_control,
	control,
	close
};


UINT32 OV8856_MIPI_RAW_SensorInit(struct SENSOR_FUNCTION_STRUCT **pfFunc)
{
	/* To Do : Check Sensor status here */
	if (pfFunc != NULL)
		*pfFunc = &sensor_func;
	return ERROR_NONE;
}	/*	OV8856_MIPI_RAW_SensorInit	*/
