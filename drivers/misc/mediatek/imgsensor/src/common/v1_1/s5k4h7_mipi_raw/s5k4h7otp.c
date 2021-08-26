/*
 * Copyright (C) 2019 MediaTek Inc.
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
/*
 * NOTE:
 * The modification is appended to initialization of image sensor.
 * After sensor initialization, use the function
 * bool otp_update_wb(unsigned char golden_rg, unsigned char golden_bg)
 * and
 * bool otp_update_lenc(void)
 * and then the calibration of AWB & LSC & BLC will be applied.
 * After finishing the OTP written, we will provide you the typical
 * value of golden sample.
 */

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

#ifndef VENDOR_EDIT
//#include "kd_camera_hw.h"
/*Caohua.Lin@Camera.Drv, 20180126 remove to adapt with mt6771*/
#endif
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"
#include "kd_camera_typedef.h"
#include "s5k4h7mipiraw_Sensor.h"
#include "s5k4h7otp.h"

#define PFX "s5k4h7otp"
#define USHORT             unsigned short
#define BYTE               unsigned char
#define I2C_ID             0x20
#define LOG_INF(fmt, args...)   pr_info(PFX "[%s] " fmt, __func__, ##args)

struct OTP {
	unsigned short	awb_infoflag;
	unsigned short	lsc_infoflag;
	unsigned short	module_integrator_id;
	int		lsc_offset;
	int		lsc_group;
	int		awbc[30];
	int		awb_page;
	unsigned short	frgcur;
	unsigned short	fbgcur;
	unsigned int	nr_gain;
	unsigned int	ng_gain;
	unsigned int	nb_gain;
	unsigned int	ngrcur;
	unsigned int	ngbcur;
	unsigned int	ngcur;
	unsigned int	nrcur;
	unsigned int	nbcur;
	unsigned int	nggolden;
	unsigned int	nrgolden;
	unsigned int	nbgolden;
	unsigned int	ngrgolden;
	unsigned int	ngbgolden;
	unsigned int	frggolden;
	unsigned int	fbggolden;
	unsigned int	awb_checksum;
	unsigned int	lsc_checksum;
};

struct OTP s5k4h7_otp_info = {0};

static kal_uint16 read_cmos_sensor_8(kal_uint32 addr)
{
	kal_uint16 get_byte = 0;

	char pu_send_cmd[2] = {(char)(addr >> 8), (char)(addr & 0xFF) };

	iReadRegI2C(pu_send_cmd, 2, (u8 *)&get_byte, 1, I2C_ID);
	return get_byte;
}

static void write_cmos_sensor_8(kal_uint16 addr, kal_uint8 para)
{
	char pusendcmd[3] = {
	(char)(addr >> 8),
	(char)(addr & 0xFF),
	(char)(para & 0xFF)};

	iWriteRegI2C(pusendcmd, 3, I2C_ID);
}

/**********************************************************
 * s5k4h7_get_page_data
 * get page data
 * return true or false
 * ***********************************************************/
void s5k4h7_get_page_data(int pageidx, unsigned char *pdata)
{
	unsigned short get_byte = 0;
	unsigned int addr = 0x0A04;
	int i = 0;

	write_cmos_sensor_8(0x0A02, pageidx);
	write_cmos_sensor_8(0x0A00, 0x01);

	do {
		mdelay(1);
		get_byte = read_cmos_sensor_8(0x0A01);
	} while ((get_byte & 0x01) != 1);

	for (i = 0; i < 64; i++) {
		pdata[i] = read_cmos_sensor_8(addr);
		addr++;
	}

	write_cmos_sensor_8(0x0A00, 0x00);
}

unsigned short s5k4h7_selective_read_region_8(int pageidx, unsigned int addr)
{
	unsigned short get_byte = 0;

	write_cmos_sensor_8(0x0A02, pageidx);
	write_cmos_sensor_8(0x0A00, 0x01);
	do {
		mdelay(1);
		get_byte = read_cmos_sensor_8(0x0A01);
	} while ((get_byte & 0x01) != 1);

	get_byte = read_cmos_sensor_8(addr);
	write_cmos_sensor_8(0x0A00, 0x00);

	return get_byte;
}

/*****************************************************
 * s5k4h7_rgb_gain_set
 * **************************************************/
void s5k4h7_rgb_gain_set(int *r_gain, int *g_gain, int *b_gain
, unsigned int r_ration, unsigned int b_ration)
{
	int gain_default = 0x0100;

		if (r_ration >= 1) {
			if (b_ration >= 1) {
				*g_gain = gain_default;
				*r_gain = (int)((gain_default*1000 * r_ration + 500)/1000);
				*b_gain = (int)((gain_default*1000 * b_ration + 500)/1000);
			} else {
				*b_gain = gain_default;
				*g_gain = (int)((gain_default * 1000 / b_ration + 500)/1000);
				*r_gain = (int)((gain_default * r_ration
					* 1000 / b_ration + 500)/1000);
			}
	} else {
		if (b_ration >= 1) {
			*r_gain = gain_default;
			*g_gain = (int)((gain_default * 1000 / r_ration + 500)/1000);
			*b_gain = (int)((gain_default * b_ration*1000 / r_ration + 500) / 1000);
		} else {
			if (r_ration >= b_ration) {
				*b_gain = gain_default;
				*g_gain = (int)((gain_default
					* 1000 / b_ration + 500) / 1000);
				*r_gain = (int)((gain_default * r_ration
					* 1000 / b_ration + 500) / 1000);
			} else {
				*r_gain = gain_default;
				*g_gain = (int)((gain_default * 1000 / r_ration + 500)/1000);
				*b_gain = (int)((gain_default * b_ration
					* 1000 / r_ration + 500) / 1000);
			}
		}
	}
}

/**********************************************************
 * s5k4h7_apply_otp_awb
 * apply otp
 * *******************************************************/
void s5k4h7_apply_otp_awb(void)
{
char r_gain_h, r_gain_l, g_gain_h, g_gain_l, b_gain_h, b_gain_l;
	unsigned int r_ratio, b_ratio;

	s5k4h7_otp_info.ngcur = (unsigned int)((s5k4h7_otp_info.ngrcur
			+ s5k4h7_otp_info.ngbcur)*1000/2 + 500);

	s5k4h7_otp_info.frgcur = (unsigned int)(s5k4h7_otp_info.nrcur*1000
			/ s5k4h7_otp_info.ngcur + 500);
	s5k4h7_otp_info.fbgcur = (unsigned int)(s5k4h7_otp_info.nbcur*1000
			/ s5k4h7_otp_info.ngcur + 500);

	s5k4h7_otp_info.nggolden = (unsigned int)((s5k4h7_otp_info.ngrgolden
			+ s5k4h7_otp_info.ngbgolden)*1000 / 2 + 500);

	s5k4h7_otp_info.frggolden = (unsigned int)(s5k4h7_otp_info.nrgolden*1000
			/ s5k4h7_otp_info.nggolden + 500);
	s5k4h7_otp_info.fbggolden = (unsigned int)(s5k4h7_otp_info.nbgolden*1000
			/ s5k4h7_otp_info.nggolden + 500);


	r_ratio = (unsigned int)((s5k4h7_otp_info.frggolden * 1000
		/ s5k4h7_otp_info.frgcur + 500)/1000);
	b_ratio = (unsigned int)((s5k4h7_otp_info.fbggolden * 1000
		/ s5k4h7_otp_info.fbgcur + 500)/1000);

	s5k4h7_rgb_gain_set(&s5k4h7_otp_info.nr_gain, &s5k4h7_otp_info.ng_gain,
		&s5k4h7_otp_info.nb_gain, r_ratio, b_ratio);

	r_gain_h = (s5k4h7_otp_info.nr_gain >> 8) & 0xff;
	r_gain_l = (s5k4h7_otp_info.nr_gain >> 0) & 0xff;

	g_gain_h = (s5k4h7_otp_info.ng_gain >> 8) & 0xff;
	g_gain_l = (s5k4h7_otp_info.ng_gain >> 0) & 0xff;

	b_gain_h = (s5k4h7_otp_info.nb_gain >> 8) & 0xff;
	b_gain_l = (s5k4h7_otp_info.nb_gain >> 0) & 0xff;

	write_cmos_sensor_8(0x0210, r_gain_h);
	write_cmos_sensor_8(0x0211, r_gain_l);

	write_cmos_sensor_8(0x020E, g_gain_h);
	write_cmos_sensor_8(0x020F, g_gain_l);

	write_cmos_sensor_8(0x0214, g_gain_h);
	write_cmos_sensor_8(0x0215, g_gain_l);

	write_cmos_sensor_8(0x0212, b_gain_h);
	write_cmos_sensor_8(0x0213, b_gain_l);

	LOG_INF("OTP apply_otp_awb\n");
}

/*********************************************************
 *s5k4h7_apply_otp_lsc
 * ******************************************************/
void s5k4h7_apply_otp_lsc(void)
{
	LOG_INF("OTP enable lsc\n");
	write_cmos_sensor_8(0x0B00, 0x01);
}

/*********************************************************
 * s5k4h7_otp_group_info
 * *****************************************************/
int s5k4h7_otp_group_info(void)
{
	int page;

	memset(&s5k4h7_otp_info, 0, sizeof(struct OTP));

	s5k4h7_otp_info.lsc_infoflag = s5k4h7_selective_read_region_8(0, 0x0A3D);

	if (s5k4h7_otp_info.lsc_infoflag == 0x01) {
		s5k4h7_otp_info.lsc_offset = 0;
		s5k4h7_otp_info.lsc_group = 1;
		s5k4h7_otp_info.lsc_checksum =
			s5k4h7_selective_read_region_8(24, 0x0A06);  //page 24
	} else if (s5k4h7_otp_info.lsc_infoflag == 0x03) {
		s5k4h7_otp_info.lsc_offset = 1;
		s5k4h7_otp_info.lsc_group = 2;
		s5k4h7_otp_info.lsc_checksum = s5k4h7_selective_read_region_8(24, 0x0A07);
	} else {
		LOG_INF("S5K4H7 OTP read data fail lsc empty!!!\n");
		goto error;
	}

	for (page = 21; page <= 23; page++) {
		s5k4h7_otp_info.awb_infoflag = s5k4h7_selective_read_region_8(page, 0x0A04);
		if ((s5k4h7_otp_info.awb_infoflag>>6 & 0x03) == 0x01) {
			s5k4h7_otp_info.awb_page = page;
			break;
		}
	}

	if ((s5k4h7_otp_info.awb_infoflag >> 6 & 0x03) != 0x01)
		goto error;

	s5k4h7_otp_info.module_integrator_id =
		s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A05);
	s5k4h7_otp_info.awb_checksum =
		s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A23);

	s5k4h7_af_inf = s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A25);
	s5k4h7_af_mac = s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A26);
	s5k4h7_af_lsb = s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A27);

	LOG_INF("AFInf = %d,AFMac = %d,AFLsb = %d\n", s5k4h7_af_inf, s5k4h7_af_mac, s5k4h7_af_lsb);

	s5k4h7_otp_info.nrcur =
		(s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A19) << 2)
		+ (s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A1D) >> 6) & 0x03;
	s5k4h7_otp_info.nbcur =
		(s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A1C) << 2)
		+ s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A1D) & 0x03;
	s5k4h7_otp_info.ngrcur =
		(s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A1A) << 2)
		+ (s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A1D) >> 4) & 0x03;
	s5k4h7_otp_info.ngbcur =
		(s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A1B) << 2)
		+ (s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A1D) >> 2) & 0x03;

	s5k4h7_otp_info.nrgolden =
		(s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A1E) << 2)
		+ (s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A18) >> 6) & 0x03;
	s5k4h7_otp_info.nbgolden =
		(s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A21) << 2)
		+ (s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A18) >> 4) & 0x03;
	s5k4h7_otp_info.ngrgolden =
		(s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A1F) << 2)
		+ (s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A18) >> 4) & 0x03;
	s5k4h7_otp_info.ngbgolden =
		(s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A20) << 2)
		+ (s5k4h7_selective_read_region_8(s5k4h7_otp_info.awb_page, 0x0A18) >> 2) & 0x03;

	return  0;
error:
	return  -1;
}

/*********************************************************
 * s5k4h7_read_page
 * read_Page1~Page21 of data
 * return true or false
 ********************************************************/
bool s5k4h7_read_page(int page_start, int page_end, unsigned char *pdata)
{
	bool bresult = true;
	int st_page_start = page_start;

	if (page_start <= 0 || page_end > 21) {
		bresult = false;
		LOG_INF(" OTP page_end is large!");
		return bresult;
	}
	for (; st_page_start <= page_end; st_page_start++)
		s5k4h7_get_page_data(st_page_start, pdata);
	return bresult;
}


unsigned int s5k4h7_checksum_lsc_flag(unsigned int sum_start,
		unsigned int sum_end, unsigned char *pdata)
{
	int i = 0;
	unsigned int start;
	unsigned int re_sum = 0;

	for (start = 0x0A04; i < 64; i++, start++) {
		if ((start >= sum_start) && (start <= sum_end))
			re_sum += pdata[i];
	}
	return  re_sum;
}


bool S5K4H7_checksum_awb(void)
{
	int i;

	bool bresult = true;
	unsigned int  checksum_awb = 0;

	for (i = 0; i <= 29; i++) {
		s5k4h7_otp_info.awbc[i] = read_cmos_sensor_8(0x0A05 + i);
		checksum_awb += s5k4h7_otp_info.awbc[i];
	}
	checksum_awb = (checksum_awb) % 255 + 1;

	LOG_INF("checksum_awb = %d,s5k4h7_otp_info.checksum_awb = %d",
		checksum_awb, s5k4h7_otp_info.awb_checksum);

	if (checksum_awb == s5k4h7_otp_info.awb_checksum) {
		s5k4h7_apply_otp_awb();
	} else {
		LOG_INF("S5K4H7 OTP checksum awb flag sum fail!!!");
		bresult &= 0;
	}
	return  bresult;
}


bool S5K4H7_checksum_lsc(void)
{
	int page_start = 21, page_end = 21;
	unsigned char data_p[21][64] = {};
	bool bresult = true;
	unsigned int  checksum_lsc = 0;

	if (s5k4h7_otp_info.lsc_group == 1) {
		for (page_start = 1, page_end = 6; page_start <= page_end; page_start++) {

			bresult &= s5k4h7_read_page(page_start, page_start, data_p[page_start-1]);
			if (page_start == 6) {
				checksum_lsc +=
				s5k4h7_checksum_lsc_flag(0x0A04, 0x0A2B, data_p[page_start-1]);
				continue;
			}
			checksum_lsc +=
				s5k4h7_checksum_lsc_flag(0x0A04, 0X0A43, data_p[page_start-1]);
		}
	} else if (s5k4h7_otp_info.lsc_group == 2) {
		for (page_start = 6, page_end = 12; page_start <= page_end; page_start++) {
			bresult &= s5k4h7_read_page(page_start, page_start, data_p[page_start-1]);
			if (page_start == 6) {
				checksum_lsc += s5k4h7_checksum_lsc_flag(0x0A2C,
					0x0A43, data_p[page_start-1]);
				continue;
			} else if (page_start < 12) {
				checksum_lsc += s5k4h7_checksum_lsc_flag(0x0A04,
					0X0A43, data_p[page_start-1]);
			} else {
				checksum_lsc += s5k4h7_checksum_lsc_flag(0x0A04,
					0X0A13, data_p[page_start-1]);
			}
		}
	}
	checksum_lsc = (checksum_lsc) % 255 + 1;
	LOG_INF("checksum_lsc = %d,s5k4h7_otp_info.checksum_lsc = %d",
		checksum_lsc, s5k4h7_otp_info.lsc_checksum);

	if (checksum_lsc == s5k4h7_otp_info.lsc_checksum) {
		s5k4h7_apply_otp_lsc();
	} else {
		LOG_INF("S5K4H7 OTP checksum lsc sum fail!!!");
		bresult &= 0;
	}

	return  bresult;
}


bool S5K4H7_otp_update(void)
{
	int result = 1;

	if (s5k4h7_otp_group_info() == -1) {
		LOG_INF("OTP read data fail  empty!!!\n");
		result &= 0;
	} else {
		if (S5K4H7_checksum_awb() == 0 || S5K4H7_checksum_lsc() == 0) {
			LOG_INF("S5K4H7 OTP checksum sum fail!!!\n");
			result &= 0;
		} else {
			LOG_INF("S5K4H7 OTP checksumsum ok\n");
		}
	}
	return  result;
}
