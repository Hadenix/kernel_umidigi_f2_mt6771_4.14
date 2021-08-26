/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef _KD_IMGSENSOR_H
#define _KD_IMGSENSOR_H


#ifndef ASSERT
#define ASSERT(expr)        WARN_ON(!(expr))
#endif

#define IMGSENSORMAGIC 'i'
/* IOCTRL(inode * ,file * ,cmd ,arg ) */
/* S means "set through a ptr" */
/* T means "tell by a arg value" */
/* G means "get by a ptr" */
/* Q means "get by return a value" */
/* X means "switch G and S atomically" */
/* H means "switch T and Q atomically" */

/******************************************************************************
 *
 ******************************************************************************/

/* sensorOpen */
#define KDIMGSENSORIOC_T_OPEN \
	_IO(IMGSENSORMAGIC, 0)
/* sensorGetInfo */
#define KDIMGSENSORIOC_X_GET_CONFIG_INFO \
	_IOWR(IMGSENSORMAGIC, 5, struct IMGSENSOR_GET_CONFIG_INFO_STRUCT)

#define KDIMGSENSORIOC_X_GETINFO \
	_IOWR(IMGSENSORMAGIC, 5, struct ACDK_SENSOR_GETINFO_STRUCT)
/* sensorGetResolution */
#define KDIMGSENSORIOC_X_GETRESOLUTION \
	_IOWR(IMGSENSORMAGIC, 10, struct ACDK_SENSOR_RESOLUTION_INFO_STRUCT)
/* For kernel 64-bit */
#define KDIMGSENSORIOC_X_GETRESOLUTION2 \
	_IOWR(IMGSENSORMAGIC, 10, struct ACDK_SENSOR_PRESOLUTION_STRUCT)
/* sensorFeatureControl */
#define KDIMGSENSORIOC_X_FEATURECONCTROL \
	_IOWR(IMGSENSORMAGIC, 15, struct ACDK_SENSOR_FEATURECONTROL_STRUCT)
/* sensorControl */
#define KDIMGSENSORIOC_X_CONTROL \
	_IOWR(IMGSENSORMAGIC, 20, struct ACDK_SENSOR_CONTROL_STRUCT)
/* sensorClose */
#define KDIMGSENSORIOC_T_CLOSE \
	_IO(IMGSENSORMAGIC, 25)
/* sensorSearch */
#define KDIMGSENSORIOC_T_CHECK_IS_ALIVE \
	_IO(IMGSENSORMAGIC, 30)
/* set sensor driver */
#define KDIMGSENSORIOC_X_SET_DRIVER \
	_IOWR(IMGSENSORMAGIC, 35, struct SENSOR_DRIVER_INDEX_STRUCT)
/* get socket postion */
#define KDIMGSENSORIOC_X_GET_SOCKET_POS \
	_IOWR(IMGSENSORMAGIC, 40, u32)
/* set I2C bus */
#define KDIMGSENSORIOC_X_SET_I2CBUS \
	_IOWR(IMGSENSORMAGIC, 45, u32)
/* set I2C bus */
#define KDIMGSENSORIOC_X_RELEASE_I2C_TRIGGER_LOCK \
	_IO(IMGSENSORMAGIC, 50)
/* Set Shutter Gain Wait Done */
#define KDIMGSENSORIOC_X_SET_SHUTTER_GAIN_WAIT_DONE \
	_IOWR(IMGSENSORMAGIC, 55, u32)
/* set mclk */
#define KDIMGSENSORIOC_X_SET_MCLK_PLL \
	_IOWR(IMGSENSORMAGIC, 60, struct ACDK_SENSOR_MCLK_STRUCT)
#define KDIMGSENSORIOC_X_GETINFO2 \
	_IOWR(IMGSENSORMAGIC, 65, struct IMAGESENSOR_GETINFO_STRUCT)
/* set open/close sensor index */
#define KDIMGSENSORIOC_X_SET_CURRENT_SENSOR \
	_IOWR(IMGSENSORMAGIC, 70, u32)
/* set GPIO */
#define KDIMGSENSORIOC_X_SET_GPIO \
	_IOWR(IMGSENSORMAGIC, 75, struct IMGSENSOR_GPIO_STRUCT)
/* Get ISP CLK */
#define KDIMGSENSORIOC_X_GET_ISP_CLK \
	_IOWR(IMGSENSORMAGIC, 80, u32)
/* Get CSI CLK */
#define KDIMGSENSORIOC_X_GET_CSI_CLK \
	_IOWR(IMGSENSORMAGIC, 85, u32)

/* Get ISP CLK via MMDVFS*/
#define KDIMGSENSORIOC_DFS_UPDATE \
	_IOWR(IMGSENSORMAGIC, 90, unsigned int)
#define KDIMGSENSORIOC_GET_SUPPORTED_ISP_CLOCKS \
	_IOWR(IMGSENSORMAGIC, 95, struct IMAGESENSOR_GET_SUPPORTED_ISP_CLK)
#define KDIMGSENSORIOC_GET_CUR_ISP_CLOCK \
	_IOWR(IMGSENSORMAGIC, 100, unsigned int)

#ifdef CONFIG_COMPAT
#define COMPAT_KDIMGSENSORIOC_X_GET_CONFIG_INFO \
	_IOWR(IMGSENSORMAGIC, 5, struct COMPAT_IMGSENSOR_GET_CONFIG_INFO_STRUCT)

#define COMPAT_KDIMGSENSORIOC_X_GETINFO \
	_IOWR(IMGSENSORMAGIC, 5, struct COMPAT_ACDK_SENSOR_GETINFO_STRUCT)
#define COMPAT_KDIMGSENSORIOC_X_FEATURECONCTROL \
	_IOWR(IMGSENSORMAGIC, 15, \
		struct COMPAT_ACDK_SENSOR_FEATURECONTROL_STRUCT)
#define COMPAT_KDIMGSENSORIOC_X_CONTROL \
	_IOWR(IMGSENSORMAGIC, 20, struct COMPAT_ACDK_SENSOR_CONTROL_STRUCT)
#define COMPAT_KDIMGSENSORIOC_X_GETINFO2 \
	_IOWR(IMGSENSORMAGIC, 65, struct COMPAT_IMAGESENSOR_GETINFO_STRUCT)
#define COMPAT_KDIMGSENSORIOC_X_GETRESOLUTION2 \
	_IOWR(IMGSENSORMAGIC, 10, struct COMPAT_ACDK_SENSOR_PRESOLUTION_STRUCT)
#endif

/************************************************************************
 *
 ************************************************************************/
/* SENSOR CHIP VERSION */
/*IMX*/
#define HM1062_MONO_SENSOR_ID                   0x1061
#define HM1062_2_MONO_SENSOR_ID                 0x1062
#define IMX686_SENSOR_ID                        0x0686
#define IMX616_SENSOR_ID                        0x0616
#define IMX499_SENSOR_ID                        0x0499
#define IMX481_SENSOR_ID                        0x0481
#define IMX486_SENSOR_ID                        0x0486
#define IMX586_SENSOR_ID                        0x0586
#define IMX586NEW_SENSOR_ID                     0x0587
#define IMX586NEW2_SENSOR_ID                     0x0588
#define IMX519_SENSOR_ID                        0x0519
#define IMX576_SENSOR_ID                        0x0576
#define IMX350_SENSOR_ID                        0x0350
#define IMX398_SENSOR_ID                        0x0398
#define IMX268_SENSOR_ID                        0x0268
#define IMX386_SENSOR_ID                        0x0386
#define IMX300_SENSOR_ID                        0x0300
#define IMX386_MONO_SENSOR_ID                   0x0286
#define IMX362_SENSOR_ID                        0x0362
#define IMX338_SENSOR_ID                        0x0338
#define IMX376_SENSOR_ID                        0x0376
#define IMX376W_SENSOR_ID                     	0x0377
#define IMX328_SENSOR_ID                        0x0091
#define IMX318_SENSOR_ID                        0x0318
#define IMX319_SENSOR_ID                        0x0319
#define IMX377_SENSOR_ID                        0x0377
#define IMX278_SENSOR_ID                        0x0278
#define IMX258_SENSOR_ID                        0x0258
#define IMX258_SENSOR_ID_SUB                    0x025a
#define IMX258_MONO_SENSOR_ID                   0x0259
#define IMX230_SENSOR_ID                        0x0230
#define IMX220_SENSOR_ID                        0x0220
#define IMX219_SENSOR_ID                        0x0219
#define IMX149_SENSOR_ID                        0x0112
#define IMX175_SENSOR_ID                        0x0112
#define IMX215_SENSOR_ID                        0x0215
#define IMX214_SENSOR_ID                        0x0214
#define IMX214_MONO_SENSOR_ID                   0x0215
#define IMX179_SENSOR_ID                        0x0179
#define IMX175_REAL_SENSOR_ID                   0x0175
#define IMX178_SENSOR_ID                        0x0178
#define IMX135_SENSOR_ID                        0x0135
#define IMX134_SENSOR_ID                        0x0134
#define IMX132MIPI_SENSOR_ID                    0x0132
#define IMX119_SENSOR_ID                        0x0119
#define IMX105_SENSOR_ID                        0x0105
#define IMX091_SENSOR_ID                        0x0091
#define IMX073_SENSOR_ID                        0x0046
#define IMX058_SENSOR_ID                        0x0058
#define IMX134_SENSOR_ID                        0x0134
#define IMX164_SENSOR_ID                        0x0164
#define IMX145_SENSOR_ID                        0x0145
#define IMX190_SENSOR_ID                        0x0190
#define IMX190_SENSOR_ID_SUB                    0x0191
#define IMX237_SENSOR_ID                        0x0237
#define IMX616_SENSOR_ID                        0x0616
#define IMX582_SENSOR_ID                        0x0582
/*AR*/
#define AR0543MIPI_SENSOR_ID                    0x4801
/*OV*/
#define OV9282_MONO_SENSOR_ID                   0x9281
#define OV9282_2_MONO_SENSOR_ID                 0x9282
#define OV23850_SENSOR_ID                       0x023850
#define OV16880_SENSOR_ID                       0x016880
#define OV16825MIPI_SENSOR_ID                   0x016820
#define OV13B10_SENSOR_ID                       0x560D42
#define OV13855_SENSOR_ID                       0xD855
#define OV13850_SENSOR_ID                       0xD850
#define OV12A10_SENSOR_ID                       0x1241
#define OV13870_SENSOR_ID                       0x013870
#define OV13850_SENSOR_ID                       0xD850
#define OV13850_SUNNY_SENSOR_ID					0xD851
#define OV13855_SENSOR_ID                       0xD855
#define OV16885_SENSOR_ID                       0x16885
#define OV16885NEW_SENSOR_ID                       0x16886
#define OV13855MAIN2_SENSOR_ID                  0xD856
#define OV12A10_SENSOR_ID 0x1241
#define OV12830_SENSOR_ID                       0xC830
#define OV9760MIPI_SENSOR_ID                    0x9760
#define OV9762MIPI_SENSOR_ID                    0x9762
#define OV9762MIPI_SENSOR_ID_MAIN               0x9763
#define OV9740MIPI_SENSOR_ID                    0x9740
#define OV9726_SENSOR_ID                        0x9726
#define OV9726MIPI_SENSOR_ID                    0x9726
#define OV8865_SENSOR_ID                        0x8865
#define OV8858_SENSOR_ID                        0x8858
#define OV8858S_SENSOR_ID                      (0x8858+1)
#define OV8856_SENSOR_ID                        0x885A
#define OV8830_SENSOR_ID                        0x8830
#define OV8825_SENSOR_ID                        0x8825
#define OV7675_SENSOR_ID                        0x7673
#define OV5693_SENSOR_ID                        0x5690
#define OV5693_MAIN_SENSOR_ID                   0x5691
#define OV5670MIPI_SENSOR_ID                    0x5670
#define OV5670MIPI_SENSOR_ID_2                  (0x5670+010000)
#define OV2281MIPI_SENSOR_ID                    0x5670
#define OV5675MIPI_SENSOR_ID                    0x5675
#define OV5671MIPI_SENSOR_ID                    0x5671
#define OV5650_SENSOR_ID                        0x5651
#define OV5650MIPI_SENSOR_ID                    0x5651
#define OV5648MIPI_SENSOR_ID                    0x5648
#define OV5648MIPI_SENSOR_ID_FRONT              0x5649
#define OV5647_SENSOR_ID                        0x5647
#define OV5647MIPI_SENSOR_ID                    0x5647
#define OV5645MIPI_SENSOR_ID                    0x5645
#define OV5642_SENSOR_ID                        0x5642
#define OV48B_SENSOR_ID                         0x564842
#define OV4689_SENSOR_ID                        0x4688
#define OV4688MIPI_SENSOR_ID                    0x4688
#define OV3640_SENSOR_ID                        0x364C
#define OV2724MIPI_SENSOR_ID                    0x2724
#define OV2722MIPI_SENSOR_ID                    0x2722
#define OV2680MIPI_SENSOR_ID                    0x2680
#define OV2680_SENSOR_ID                        0x2680
#define OV2659_SENSOR_ID                        0x2656
#define OV2650_SENSOR_ID                        0x2652
#define OV2650_SENSOR_ID_1                      0x2651
#define OV2650_SENSOR_ID_2                      0x2652
#define OV2650_SENSOR_ID_3                      0x2655
#define OV13853_SENSOR_ID                       0xD853
#define OV13870_SENSOR_ID                       0x013870
#define OV20880MIPI_SENSOR_ID                   0x20880
#define OV48C_SENSOR_ID                         0x564843
#define OV05A20_SENSOR_ID	                    0x5305
#define OV13B10_SENSOR_ID                       0x560D42
#define OV02B_SENSOR_ID                         0x002b
/*S5K*/
#define S5KGW1_SENSOR_ID                        0x0971
#define S5KGW1NEW_SENSOR_ID                        0x0972
#define S5KGM1SP_SENSOR_ID                      0x08d1
#define S5KGM1SPB_SENSOR_ID                     0x08d2
#define S5KGM1CTS_SENSOR_ID                     0x08d3
#define S5KGM1ST_SENSOR_ID                      0xf8d1
#define S5KGM1GZ_SENSOR_ID                      0xf8d2
#define S5KGD1SP_SENSOR_ID                      0x0841
#define S5KJD1_SENSOR_ID                        0x3841
#define S5K2XASP_SENSOR_ID                      0x218A
#define S5K2LQSX_SENSOR_ID                      0x2c1a
#define S5K4H7_SENSOR_ID                        0x487B
#define S5K4H7CTS_SENSOR_ID                     0x487C
#define S5K4H7YXSUB_SENSOR_ID                   0x487A
#define S5K3P8SP_SENSOR_ID                      0x3108
#define S5K2T7SP_SENSOR_ID                      0x2147
#define S5K3P8SX_SENSOR_ID                      0x3108
#define S5K2L7_SENSOR_ID                        0x20C7
#define S5K3L6_SENSOR_ID                        0x30C6
#define S5K3L8_SENSOR_ID                        0x30C8
#define S5K3M3_SENSOR_ID                        0x30D3
#define S5K3M5SX_SENSOR_ID                      0x30D5
#define S5K2X8_SENSOR_ID                        0x2188
#define S5K2X5SP_SENSOR_ID                      0x2185
#define S5K2P7_SENSOR_ID                        0x2107
#define S5K3P9_SENSOR_ID                        0x3109
#define S5K2P8_SENSOR_ID                        0x2108
#define S5K3P3_SENSOR_ID                        0x3103
#define S5K3P3SX_SENSOR_ID                      0x3103
#define S5K3P9SP_SENSOR_ID                      0x3109
#define S5K3P8_SENSOR_ID                        0x3108
#define S5K3P8STECH_SENSOR_ID                   0xf3108
#define S5K3M2_SENSOR_ID                        0x30D2
#define S5K3M2_SENSOR_ID_SUB                    0x30D1
#define S5K4E6_SENSOR_ID                        0x4e60
#define S5K3AAEA_SENSOR_ID                      0x07AC
#define S5K3BAFB_SENSOR_ID                      0x7070
#define S5K3H7Y_SENSOR_ID                       0x3087
#define S5K3H2YX_SENSOR_ID                      0x382b
#define S5KA3DFX_SENSOR_ID                      0x00AB
#define S5K3E2FX_SENSOR_ID                      0x3E2F
#define S5K4B2FX_SENSOR_ID                      0x5080
#define S5K4E1GA_SENSOR_ID                      0x4E10
#define S5K4E2GX_SENSOR_ID                      0x4E20
#define S5K4ECGX_SENSOR_ID                      0x4EC0
#define S5K53BEX_SENSOR_ID                      0x45A8
#define S5K53BEB_SENSOR_ID                      0x87A8
#define S5K5BAFX_SENSOR_ID                      0x05BA
#define S5K5E2YA_SENSOR_ID                      0x5e20
#define S5K4H5YX_2LANE_SENSOR_ID                0x485B
#define S5K4H5YC_SENSOR_ID                      0x485B
#define S5K3H7YX_SENSOR_ID                      0x3087
#define S5K4H8_SENSOR_ID                        0x4088
#define S5K83AFX_SENSOR_ID                      0x01C4
#define S5K5CAGX_SENSOR_ID                      0x05ca
#define S5K8AAYX_MIPI_SENSOR_ID                 0x08aa
#define S5K8AAYX_SENSOR_ID                      0x08aa
#define S5K5E8YX_SENSOR_ID                      0x5e80
#define S5K5E8YXREAR2_SENSOR_ID                 0x5e81
#define S5K5E9_SENSOR_ID                        0x559b
#define S5K3H5XA_SENSOR_ID                      0x3085
#define S5K3L2_SENSOR_ID                        0x30c2
#define S5KHM2SP_SENSOR_ID                      0x1AD2
#define S5K2T7_SENSOR_ID                        0x2147
/*HI*/
#define HI1332_SENSOR_ID                        0x1332
#define HI1333_SENSOR_ID                        0x1333
#define HI841_SENSOR_ID                         0x0841
#define HI707_SENSOR_ID                         0x00b8
#define HI704_SENSOR_ID                         0x0096
#define HI556_SENSOR_ID                         0x0556
#define HI551_SENSOR_ID                         0x0551
#define HI553_SENSOR_ID                         0x0553
#define HI545MIPI_SENSOR_ID                     0x0545
#define HI544MIPI_SENSOR_ID                     0x0544
#define HI542_SENSOR_ID                         0x00B1
#define HI542MIPI_SENSOR_ID                     0x00B1
#define HI253_SENSOR_ID                         0x0092
#define HI251_SENSOR_ID                         0x0084
#define HI191MIPI_SENSOR_ID                     0x0191
#define HI842_SENSOR_ID						    0x0842
#define HI843B_SENSOR_ID                        0x0843
#define HI843_SENSOR_ID                         0x0843
#define HI258_SENSOR_ID 						0x00d1
#define HI258_SENSOR_SUB_ID 				    0x00d2
#define HI259MIPI_SENSOR_ID                     0xE1
#define HIVICF_SENSOR_ID                        0x0081
#define HI556_SENSOR_ID                         0x0556
#define HI556_SENSOR_SUB_ID                     0x0557
#define HI846_SENSOR_ID                         0x0846
#define HI846WIDE_SENSOR_ID                     0x0848
#define HI846_SENSOR_ID_SUB                     0x0847
#define HI2907_SENSOR_ID                        0x202
#define MT9D011_SENSOR_ID                       0x1511
#define MT9D111_SENSOR_ID                       0x1511
#define MT9D112_SENSOR_ID                       0x1580
#define MT9M011_SENSOR_ID                       0x1433
#define MT9M111_SENSOR_ID                       0x143A
#define MT9M112_SENSOR_ID                       0x148C
#define MT9M113_SENSOR_ID                       0x2480
#define MT9P012_SENSOR_ID                       0x2800
#define MT9P012_SENSOR_ID_REV7                  0x2801
#define MT9T012_SENSOR_ID                       0x1600
#define MT9T013_SENSOR_ID                       0x2600
#define MT9T113_SENSOR_ID                       0x4680
#define MT9V112_SENSOR_ID                       0x1229
#define MT9DX11_SENSOR_ID                       0x1519
#define MT9D113_SENSOR_ID                       0x2580
#define MT9D115_SENSOR_ID                       0x2580
#define MT9D115MIPI_SENSOR_ID                   0x2580
#define MT9V113_SENSOR_ID                       0x2280
#define MT9V114_SENSOR_ID                       0x2283
#define MT9V115_SENSOR_ID                       0x2284
#define MT9P015_SENSOR_ID                       0x2803
#define MT9P017_SENSOR_ID                       0x4800
#define MT9P017MIPI_SENSOR_ID                   0x4800
#define MT9T113MIPI_SENSOR_ID                   0x4680
/*GC*/
#define GC8054_SENSOR_ID                        0x8054
#define GC5035_SENSOR_ID                        0x5035
#define GC2375_SENSOR_ID                        0x2375
#define GC2375H_SENSOR_ID                       0x2375
#define GC2375SUB_SENSOR_ID                     0x2376
#define GC2365_SENSOR_ID                        0x2365
#define GC2366_SENSOR_ID                        0x2366
#define GC2355_SENSOR_ID                        0x2355
#define GC2355SUB_SENSOR_ID                     0x2356
#define GC2365MIPI_SENSOR_ID                    0x2365
#define GC2235_SENSOR_ID                        0x2235
#define GC2035_SENSOR_ID                        0x2035
#define GC2145_SENSOR_ID                        0x2145
#define GC2155MIPI_SENSOR_ID                    0x2155
#define GC0330_SENSOR_ID                        0xC1
#define GC0329_SENSOR_ID                        0xC0
#define GC0310_SENSOR_ID                        0xa310
#define GC0313MIPI_YUV_SENSOR_ID                0xD0
#define GC0409MIPI_SENSOR_ID                    0x0409
#define GC5004MIPI_SENSOR_ID                    0x5004
#define GC5005MIPI_SENSOR_ID                    0x5005
#define GC5005MIPI_SENSOR_ID_SUB                0x5006
#define GC5024MIPI_SENSOR_ID                    0x5024
#define GC8024_SENSOR_ID                        0x8024
#define GC0312_SENSOR_ID                        0xb310
#define GC030AMIPI_SENSOR_ID                    0x030a
#define GC030AMIPI_SENSOR_ID_FRONT				0x030b
#define GC2235_SENSOR_ID                        0x2235
#define GC2235_SENSOR_SUB_ID			 		0x2236
#define GC5025MIPI_SENSOR_ID					0x5025
#define GC5025MIPI_SENSOR_FRONT_ID				0x5026
#define GC5035MIPI_SENSOR_ID					0x5035
#define GC5035MIPI_SENSOR_SUB_ID 				0x5036
#define GC033AMIPI_SENSOR_ID				    0x033a
#define GC5024MIPI_SENSOR_ID                    0x5024
#define GC8024MIPI_2LAN_SENSOR_ID               0x8025
#define GC2385MIPI_SENSOR_ID					0x2385
#define GC2385MIPI_SENSOR_SUB_ID                0x2386
#define GC13023_SENSOR_ID                       0x1323
#define GC8034_SENSOR_ID                        0x8044
#define GC8C34_SENSOR_ID                        0x80C4
#define GC8034MAIN_SENSOR_ID                    0x8045
#define GC6153_SENSOR_ID                        0x6153
#define GC08A3_SENSOR_ID                        0x08A3
/*SP*/
#define SP0A19_YUV_SENSOR_ID                    0xA6
#define SP2518_YUV_SENSOR_ID                    0x53
#define SP2509_SENSOR_ID                        0x2509
#define SP250AMIPI_SENSOR_ID 					0x250a
#define SP250AMIPI_SENSOR_ID_FRONT				0x250b
#define SP2509MIPI_SENSOR_SUB_ID				0x2510
#define SP2609MIPI_SENSOR_ID                    0x2608
#define SP2609MIPI_SENSOR_SUB_ID                0x2609
#define SP0A09MIPI_SENSOR_ID 					0x0a09
#define SP0A20_YUV_SENSOR_ID 					0x002b
#define SP5508MIPI_SENSOR_ID 					0x5508
#define SP2518_YUV_SENSOR_ID                    0x53
#define SP5409MIPI_SENSOR_ID 					0x5409
#define SP250A_SENSOR_ID                        0x250A
#define SP2508_SENSOR_ID 						0x2508
#define SP8408MIPI_SENSOR_ID 					0x1490
#define SP540AMIPI_SENSOR_ID 					0x540a
#define SP5506MIPI_SENSOR_ID                    0x5676
/*A*/
#define A5141MIPI_SENSOR_ID                     0x4800
#define A5142MIPI_SENSOR_ID                     0x4800
/*HM*/
#define HM3451_SENSOR_ID                        0x345
#define HM5040_SENSOR_ID                        0x03bb  //zxs 20150209
#define HM8030_SENSOR_ID 						0xa64
#define HM8040MIPI_SENSOR_ID                    0x8040
/*AR*/
#define AR0833_SENSOR_ID                        0x4B03
#define AR1335_SENSOR_ID                        0x153
/*BF*/
#define BF2206MIPI_SENSOR_ID					0x2206
#define BF2206MIPI_SENSOR_ID_SUB				0x2207
#define BF20A1MIPI_SENSOR_ID					0x20a1
/*VD*/
#define VD6955MIPI_SENSOR_ID					0x03bc//0x6955
/*SIV*/
#define SID020A_SENSOR_ID                       0x12B4
#define SIV100B_SENSOR_ID                       0x0C11
#define SIV100A_SENSOR_ID                       0x0C10
#define SIV120A_SENSOR_ID                       0x1210
#define SIV120B_SENSOR_ID                       0x0012
#define SIV121D_SENSOR_ID                       0xDE
#define SIM101B_SENSOR_ID                       0x09A0
#define SIM120C_SENSOR_ID                       0x0012
#define SID130B_SENSOR_ID                       0x001b
#define SIC110A_SENSOR_ID                       0x000D
#define SIV120B_SENSOR_ID                       0x0012
/*PAS (PixArt Image)*/
#define PAS105_SENSOR_ID                        0x0065
#define PAS302_SENSOR_ID                        0x0064
#define PAS5101_SENSOR_ID                       0x0067
#define PAS6180_SENSOR_ID                       0x6179
/*Panasoic*/
#define MN34152_SENSOR_ID                       0x01
#define MN045_SENSOR_ID                     	0x6017
/*Toshiba*/
#define T4KA7_SENSOR_ID                         0x2c30
#define T4K37_SENSOR_ID                         0x1c21
#define T4KB3_SENSOR_ID		 				    0x1c50
/*Others*/
#define SHARP3D_SENSOR_ID                       0x003d
#define T8EV5_SENSOR_ID                         0x1011
#define BF2253_SENSOR_ID                   0x2253
/*Test*/
#define OV02A10_MONO_SENSOR_ID                  0x2509
#define GC02M0_SENSOR_ID                        0x02d0
#define GC02M0_SENSOR_ID1                       0x02d1
#define GC02M0_SENSOR_ID2                       0x02d2
#define GC02K0_SENSOR_ID                        0x2385
#define OV16A10_SENSOR_ID                       0x561641
#define GC02M1B_SENSOR_ID                       0x02e0
#define CAMERA_HW_DEVNAME "kd_camera_hw"
/* SENSOR DEVICE DRIVER NAME */
/*IMX*/
#define SENSOR_DRVNAME_HM1062_MIPI_MONO          "hm1062_mipi_mono"
#define SENSOR_DRVNAME_HM1062_2_MIPI_MONO        "hm1062_2_mipi_mono"
#define SENSOR_DRVNAME_IMX686_MIPI_RAW           "imx686_mipi_raw"
#define SENSOR_DRVNAME_IMX616_MIPI_RAW          "imx616_mipi_raw"
#define SENSOR_DRVNAME_IMX499_MIPI_RAW           "imx499_mipi_raw"
#define SENSOR_DRVNAME_IMX499_MIPI_RAW_13M       "imx499_mipi_raw_13m"
#define SENSOR_DRVNAME_IMX481_MIPI_RAW           "imx481_mipi_raw"
#define SENSOR_DRVNAME_IMX486_MIPI_RAW           "imx486_mipi_raw"
#define SENSOR_DRVNAME_IMX586_MIPI_RAW           "imx586_mipi_raw"
#define SENSOR_DRVNAME_IMX586NEW_MIPI_RAW        "imx586new_mipi_raw"
#define SENSOR_DRVNAME_IMX586NEW2_MIPI_RAW        "imx586new2_mipi_raw"
#define SENSOR_DRVNAME_IMX519_MIPI_RAW           "imx519_mipi_raw"
#define SENSOR_DRVNAME_IMX519DUAL_MIPI_RAW       "imx519dual_mipi_raw"
#define SENSOR_DRVNAME_IMX576_MIPI_RAW           "imx576_mipi_raw"
#define SENSOR_DRVNAME_IMX350_MIPI_RAW           "imx350_mipi_raw"
#define SENSOR_DRVNAME_IMX398_MIPI_RAW           "imx398_mipi_raw"
#define SENSOR_DRVNAME_IMX268_MIPI_RAW           "imx268_mipi_raw"
#define SENSOR_DRVNAME_IMX386_MIPI_RAW           "imx386_mipi_raw"
#define SENSOR_DRVNAME_IMX300_MIPI_RAW           "imx300_mipi_raw"
#define SENSOR_DRVNAME_IMX386_MIPI_MONO          "imx386_mipi_mono"
#define SENSOR_DRVNAME_IMX362_MIPI_RAW           "imx362_mipi_raw"
#define SENSOR_DRVNAME_IMX338_MIPI_RAW           "imx338_mipi_raw"
#define SENSOR_DRVNAME_IMX376_MIPI_RAW           "imx376_mipi_raw"
#define SENSOR_DRVNAME_IMX376W_MIPI_RAW          "imx376w_mipi_raw"
#define SENSOR_DRVNAME_IMX328_MIPI_RAW           "imx328_mipi_raw"
#define SENSOR_DRVNAME_IMX318_MIPI_RAW           "imx318_mipi_raw"
#define SENSOR_DRVNAME_IMX319_MIPI_RAW           "imx319_mipi_raw"
#define SENSOR_DRVNAME_IMX377_MIPI_RAW           "imx377_mipi_raw"
#define SENSOR_DRVNAME_IMX278_MIPI_RAW           "imx278_mipi_raw"
#define SENSOR_DRVNAME_IMX258_MIPI_RAW          "imx258_mipi_raw"
#define SENSOR_DRVNAME_IMX258_MIPI_RAW_SUB      "imx258_mipi_raw_sub"
#define SENSOR_DRVNAME_IMX258_MIPI_MONO         "imx258_mipi_mono"
#define SENSOR_DRVNAME_IMX230_MIPI_RAW          "imx230_mipi_raw"
#define SENSOR_DRVNAME_IMX220_MIPI_RAW          "imx220_mipi_raw"
#define SENSOR_DRVNAME_IMX175_MIPI_RAW          "imx175_mipi_raw"
#define SENSOR_DRVNAME_IMX219_MIPI_RAW          "imx219_mipi_raw"
#define SENSOR_DRVNAME_IMX214_MIPI_MONO         "imx214_mipi_mono"
#define SENSOR_DRVNAME_IMX215_MIPI_RAW          "imx215_mipi_raw"
#define SENSOR_DRVNAME_IMX214_MIPI_RAW          "imx214_mipi_raw"
#define SENSOR_DRVNAME_IMX179_MIPI_RAW          "imx179_mipi_raw"
#define SENSOR_DRVNAME_IMX179_MIPI_RAW_5MP      "imx179_mipi_raw_5mp"
#define SENSOR_DRVNAME_IMX175_REAL_MIPI_RAW     "imx175_real_mipi_raw"
#define SENSOR_DRVNAME_IMX178_MIPI_RAW          "imx178_mipi_raw"
#define SENSOR_DRVNAME_IMX135_MIPI_RAW          "imx135_mipi_raw"
#define SENSOR_DRVNAME_IMX134_MIPI_RAW          "imx134_mipi_raw"
#define SENSOR_DRVNAME_IMX132_MIPI_RAW          "imx132_mipi_raw"
#define SENSOR_DRVNAME_IMX119_MIPI_RAW          "imx119_mipi_raw"
#define SENSOR_DRVNAME_IMX105_MIPI_RAW          "imx105_mipi_raw"
#define SENSOR_DRVNAME_IMX091_MIPI_RAW          "imx091_mipi_raw"
#define SENSOR_DRVNAME_IMX073_MIPI_RAW          "imx073_mipi_raw"
#define SENSOR_DRVNAME_IMX164_MIPI_RAW          "imx164_mipi_raw"
#define SENSOR_DRVNAME_IMX134_MIPI_RAW          "imx134_mipi_raw"
#define SENSOR_DRVNAME_IMX145_MIPI_RAW          "imx145_mipi_raw"
#define SENSOR_DRVNAME_IMX190_MIPI_RAW          "imx190_mipi_raw"
#define SENSOR_DRVNAME_IMX190_MIPI_RAW_SUB      "imx190_mipi_raw_sub"
#define SENSOR_DRVNAME_IMX237_MIPI_RAW          "imx237_mipi_raw"
#define SENSOR_DRVNAME_IMX616_MIPI_RAW          "imx616_mipi_raw"
#define SENSOR_DRVNAME_IMX582_MIPI_RAW          "imx582_mipi_raw"
/*OV*/
#define SENSOR_DRVNAME_OV9282_MIPI_MONO          "ov9282_mipi_mono"
#define SENSOR_DRVNAME_OV9282_2_MIPI_MONO        "ov9282_2_mipi_mono"
#define SENSOR_DRVNAME_OV23850_MIPI_RAW         "ov23850_mipi_raw"
#define SENSOR_DRVNAME_OV16880_MIPI_RAW         "ov16880_mipi_raw"
#define SENSOR_DRVNAME_OV16885_MIPI_RAW         "ov16885_mipi_raw"
#define SENSOR_DRVNAME_OV16885NEW_MIPI_RAW "ov16885new_mipi_raw"
#define SENSOR_DRVNAME_OV16825_MIPI_RAW         "ov16825_mipi_raw"
#define SENSOR_DRVNAME_OV13B10_MIPI_RAW          "ov13b10_mipi_raw"
#define SENSOR_DRVNAME_OV13870_MIPI_RAW         "ov13870_mipi_raw"
#define SENSOR_DRVNAME_OV13855_MIPI_RAW         "ov13855_mipi_raw"
#define SENSOR_DRVNAME_OV13855MAIN2_MIPI_RAW    "ov13855main2_mipi_raw"
#define SENSOR_DRVNAME_OV13853_MIPI_RAW         "ov13853_mipi_raw"
#define SENSOR_DRVNAME_OV13850_MIPI_RAW         "ov13850_mipi_raw"
#define SENSOR_DRVNAME_OV12A10_MIPI_RAW         "ov12a10_mipi_raw"
#define SENSOR_DRVNAME_OV12830_MIPI_RAW         "ov12830_mipi_raw"
#define SENSOR_DRVNAME_OV9760_MIPI_RAW          "ov9760_mipi_raw"
#define SENSOR_DRVNAME_OV9762_MIPI_RAW          "ov9762_mipi_raw"
#define SENSOR_DRVNAME_OV9762_MIPI_RAW_MAIN     "ov9762_mipi_raw_main"
#define SENSOR_DRVNAME_OV9740_MIPI_YUV          "ov9740_mipi_yuv"
#define SENSOR_DRVNAME_0V9726_RAW               "ov9726_raw"
#define SENSOR_DRVNAME_OV9726_MIPI_RAW          "ov9726_mipi_raw"
#define SENSOR_DRVNAME_OV8865_MIPI_RAW          "ov8865_mipi_raw"
#define SENSOR_DRVNAME_OV8858_MIPI_RAW          "ov8858_mipi_raw"
#define SENSOR_DRVNAME_OV8858S_MIPI_RAW         "ov8858s_mipi_raw"
#define SENSOR_DRVNAME_OV8856_MIPI_RAW          "ov8856_mipi_raw"
#define SENSOR_DRVNAME_OV8830_RAW               "ov8830_raw"
#define SENSOR_DRVNAME_OV8825_MIPI_RAW          "ov8825_mipi_raw"
#define SENSOR_DRVNAME_OV7675_YUV               "ov7675_yuv"
#define SENSOR_DRVNAME_OV5693_MIPI_RAW          "ov5693_mipi_raw"
#define SENSOR_DRVNAME_OV5670_MIPI_RAW          "ov5670_mipi_raw"
#define SENSOR_DRVNAME_OV5670_MIPI_RAW_2        "ov5670_mipi_raw_2"
#define SENSOR_DRVNAME_OV2281_MIPI_RAW          "ov2281_mipi_raw"
#define SENSOR_DRVNAME_OV5675_MIPI_RAW          "ov5675_mipi_raw"
#define SENSOR_DRVNAME_OV5671_MIPI_RAW          "ov5671_mipi_raw"
#define SENSOR_DRVNAME_OV5647MIPI_RAW           "ov5647_mipi_raw"
#define SENSOR_DRVNAME_OV5645_MIPI_RAW          "ov5645_mipi_raw"
#define SENSOR_DRVNAME_OV5645_MIPI_YUV          "ov5645_mipi_yuv"
#define SENSOR_DRVNAME_OV5650MIPI_RAW           "ov5650_mipi_raw"
#define SENSOR_DRVNAME_OV5650_RAW               "ov5650_raw"
#define SENSOR_DRVNAME_OV5648_MIPI_RAW          "ov5648_mipi_raw"
#define SENSOR_DRVNAME_OV5648_MIPI_RAW_FRONT    "ov5648_mipi_raw_front"
#define SENSOR_DRVNAME_OV5647_RAW               "ov5647_raw"
#define SENSOR_DRVNAME_OV5642_RAW               "ov5642_raw"
#define SENSOR_DRVNAME_OV5642_MIPI_YUV          "ov5642_mipi_yuv"
#define SENSOR_DRVNAME_OV5642_MIPI_RGB          "ov5642_mipi_rgb"
#define SENSOR_DRVNAME_OV5642_MIPI_JPG          "ov5642_mipi_jpg"
#define SENSOR_DRVNAME_OV5642_YUV               "ov5642_yuv"
#define SENSOR_DRVNAME_OV5642_YUV_SWI2C         "ov5642_yuv_swi2c"
#define SENSOR_DRVNAME_OV48B_MIPI_RAW           "ov48b_mipi_raw"
#define SENSOR_DRVNAME_OV4689_MIPI_RAW          "ov4689_mipi_raw"
#define SENSOR_DRVNAME_OV4688_MIPI_RAW          "ov4688_mipi_raw"
#define SENSOR_DRVNAME_OV3640_RAW               "ov3640_raw"
#define SENSOR_DRVNAME_OV3640_YUV               "ov3640_yuv"
#define SENSOR_DRVNAME_OV2724_MIPI_RAW          "ov2724_mipi_raw"
#define SENSOR_DRVNAME_OV2722_MIPI_RAW          "ov2722_mipi_raw"
#define SENSOR_DRVNAME_OV2680_MIPI_RAW          "ov2680_mipi_raw"
#define SENSOR_DRVNAME_OV48C_MIPI_RAW           "ov48c_mipi_raw"
#define SENSOR_DRVNAME_OV2659_MIPI_YUV               "ov2659_mipi_yuv"
#define SENSOR_DRVNAME_OV2655_YUV               "ov2655_yuv"
#define SENSOR_DRVNAME_OV2650_RAW               "ov265x_raw"
#define SENSOR_DRVNAME_OV20880_MIPI_RAW         "ov20880_mipi_raw"
#define SENSOR_DRVNAME_OV13B10_MIPI_RAW         "ov13b10_mipi_raw"
#define SENSOR_DRVNAME_OV13870_MIPI_RAW         "ov13870_mipi_raw"
#define SENSOR_DRVNAME_OV05A20_MIPI_RAW         "ov05a20_mipi_raw"
#define SENSOR_DRVNAME_OV02B_MIPI_RAW           "ov02b_mipi_raw"
/*S5K*/
#define SENSOR_DRVNAME_S5KGW1_MIPI_RAW          "s5kgw1_mipi_raw"
#define SENSOR_DRVNAME_S5KGW1NEW_MIPI_RAW          "s5kgw1new_mipi_raw"
#define SENSOR_DRVNAME_S5KGM1SP_MIPI_RAW        "s5kgm1sp_mipi_raw"
#define SENSOR_DRVNAME_S5KGM1SPB_MIPI_RAW       "s5kgm1spb_mipi_raw"
#define SENSOR_DRVNAME_S5KGM1CTS_MIPI_RAW       "s5kgm1cts_mipi_raw"
#define SENSOR_DRVNAME_S5KGM1ST_MIPI_RAW        "s5kgm1st_mipi_raw"
#define SENSOR_DRVNAME_S5KGM1GZ_MIPI_RAW        "s5kgm1gz_mipi_raw"
#define SENSOR_DRVNAME_S5KGD1SP_MIPI_RAW        "s5kgd1sp_mipi_raw"
#define SENSOR_DRVNAME_S5KJD1_MIPI_RAW          "s5kjd1_mipi_raw"
#define SENSOR_DRVNAME_S5K2XASP_MIPI_RAW        "s5k2xasp_mipi_raw"
#define SENSOR_DRVNAME_S5K2LQSX_MIPI_RAW        "s5k2lqsx_mipi_raw"
#define SENSOR_DRVNAME_S5K4H7_MIPI_RAW          "s5k4h7_mipi_raw"
#define SENSOR_DRVNAME_S5K4H7CTS_MIPI_RAW       "s5k4h7cts_mipi_raw"
#define SENSOR_DRVNAME_S5K4H7YXSUB_MIPI_RAW     "s5k4h7yxsub_mipi_raw"
#define SENSOR_DRVNAME_S5K3P8SP_MIPI_RAW        "s5k3p8sp_mipi_raw"
#define SENSOR_DRVNAME_S5K3P8SX_MIPI_RAW        "s5k3p8sx_mipi_raw"
#define SENSOR_DRVNAME_S5K2T7SP_MIPI_RAW        "s5k2t7sp_mipi_raw"
#define SENSOR_DRVNAME_S5K2T7SP_MIPI_RAW_5M     "s5k2t7sp_mipi_raw_5m"
#define SENSOR_DRVNAME_S5K3P8SX_MIPI_RAW        "s5k3p8sx_mipi_raw"
#define SENSOR_DRVNAME_S5K2L7_MIPI_RAW          "s5k2l7_mipi_raw"
#define SENSOR_DRVNAME_S5K3L6_MIPI_RAW          "s5k3l6_mipi_raw"
#define SENSOR_DRVNAME_S5K3L8_MIPI_RAW          "s5k3l8_mipi_raw"
#define SENSOR_DRVNAME_S5K3M3_MIPI_RAW          "s5k3m3_mipi_raw"
#define SENSOR_DRVNAME_S5K3M5SX_MIPI_RAW        "s5k3m5sx_mipi_raw"
#define SENSOR_DRVNAME_S5K2X8_MIPI_RAW          "s5k2x8_mipi_raw"
#define SENSOR_DRVNAME_S5K2X5SP_MIPI_RAW        "s5k2x5sp_mipi_raw"
#define SENSOR_DRVNAME_S5K2P7_MIPI_RAW          "s5k2p7_mipi_raw"
#define SENSOR_DRVNAME_S5K3P9_MIPI_RAW          "s5k3p9_mipi_raw"
#define SENSOR_DRVNAME_S5K2P8_MIPI_RAW          "s5k2p8_mipi_raw"
#define SENSOR_DRVNAME_S5K3P3SX_MIPI_RAW        "s5k3p3sx_mipi_raw"
#define SENSOR_DRVNAME_S5K3P3_MIPI_RAW          "s5k3p3_mipi_raw"
#define SENSOR_DRVNAME_S5K3P9SP_MIPI_RAW        "s5k3p9sp_mipi_raw"
#define SENSOR_DRVNAME_S5K3P8_MIPI_RAW          "s5k3p8_mipi_raw"
#define SENSOR_DRVNAME_S5K3M2_MIPI_RAW          "s5k3m2_mipi_raw"
#define SENSOR_DRVNAME_S5K3M2_MIPI_RAW_SUB      "s5k3m2_mipi_raw_sub"
#define SENSOR_DRVNAME_S5K4E6_MIPI_RAW          "s5k4e6_mipi_raw"
#define SENSOR_DRVNAME_S5K3H2YX_MIPI_RAW        "s5k3h2yx_mipi_raw"
#define SENSOR_DRVNAME_S5K3H7Y_MIPI_RAW         "s5k3h7y_mipi_raw"
#define SENSOR_DRVNAME_S5K4H5YC_MIPI_RAW        "s5k4h5yc_mipi_raw"
#define SENSOR_DRVNAME_S5K3H7YX_MIPI_RAW        "s5k3h7yx_mipi_raw"
#define SENSOR_DRVNAME_S5K4H8_MIPI_RAW          "s5k4h8_mipi_raw"
#define SENSOR_DRVNAME_S5K4E1GA_MIPI_RAW        "s5k4e1ga_mipi_raw"
#define SENSOR_DRVNAME_S5K4E2GX_MIPI_RAW        "s5k4e2gx_mipi_raw"
#define SENSOR_DRVNAME_S5K4ECGX_MIPI_YUV        "s5k4ecgx_mipi_yuv"
#define SENSOR_DRVNAME_S5K5CAGX_YUV             "s5k5cagx_yuv"
#define SENSOR_DRVNAME_S5K4H5YX_2LANE_MIPI_RAW  "s5k4h5yx_2lane_mipi_raw"
#define SENSOR_DRVNAME_S5K5E2YA_MIPI_RAW        "s5k5e2ya_mipi_raw"
#define SENSOR_DRVNAME_S5K8AAYX_MIPI_YUV        "s5k8aayx_mipi_yuv"
#define SENSOR_DRVNAME_S5K8AAYX_YUV             "s5k8aayx_yuv"
#define SENSOR_DRVNAME_S5K5E8YX_MIPI_RAW        "s5k5e8yx_mipi_raw"
#define SENSOR_DRVNAME_S5K5E8YXREAR2_MIPI_RAW   "s5k5e8yxrear2_mipi_raw"
#define SENSOR_DRVNAME_S5K5E9_MIPI_RAW          "s5k5e9_mipi_raw"
#define SENSOR_DRVNAME_S5K3H5XA_MIPI_RAW        "s5k3h5xa_mipi_raw"
#define SENSOR_DRVNAME_S5K3L2_MIPI_RAW		    "s5k3l2_mipi_raw"
#define SENSOR_DRVNAME_S5KHM2SP_MIPI_RAW        "s5khm2sp_mipi_raw"
#define SENSOR_DRVNAME_S5K2T7_MIPI_RAW          "s5k2t7_mipi_raw"
/*HI*/
#define SENSOR_DRVNAME_HI1332_MIPI_RAW          "hi1332_mipi_raw"
#define SENSOR_DRVNAME_HI1333_MIPI_RAW           "hi1333_mipi_raw"
#define SENSOR_DRVNAME_HI841_MIPI_RAW           "hi841_mipi_raw"
#define SENSOR_DRVNAME_HI707_YUV                "hi707_yuv"
#define SENSOR_DRVNAME_HI704_YUV                "hi704_yuv"
#define SENSOR_DRVNAME_HI556_MIPI_RAW           "hi556_mipi_raw"
#define SENSOR_DRVNAME_HI551_MIPI_RAW           "hi551_mipi_raw"
#define SENSOR_DRVNAME_HI553_MIPI_RAW           "hi553_mipi_raw"
#define SENSOR_DRVNAME_HI545_MIPI_RAW           "hi545_mipi_raw"
#define SENSOR_DRVNAME_HI542_RAW                "hi542_raw"
#define SENSOR_DRVNAME_HI542MIPI_RAW            "hi542_mipi_raw"
#define SENSOR_DRVNAME_HI544_MIPI_RAW           "hi544_mipi_raw"
#define SENSOR_DRVNAME_HI253_YUV                "hi253_yuv"
#define SENSOR_DRVNAME_HI191_MIPI_RAW           "hi191_mipi_raw"
#define SENSOR_DRVNAME_HI846_MIPI_RAW           "hi846_mipi_raw"
#define SENSOR_DRVNAME_HI846_MIPI_RAW_SUB       "hi846_mipi_raw_sub"
#define SENSOR_DRVNAME_HI842_MIPI_RAW		    "hi842_mipi_raw"
#define SENSOR_DRVNAME_HI843B_MIPI_RAW          "hi843b_mipi_raw"
#define SENSOR_DRVNAME_HI843_MIPI_RAW           "hi843_mipi_raw"
#define SENSOR_DRVNAME_HI259_MIPI_RAW           "hi259_mipi_raw"
#define SENSOR_DRVNAME_HI258_MIPI_YUV           "hi258_mipi_yuv"
#define SENSOR_DRVNAME_HI258_MIPI_YUV_SUB       "hi258_mipi_yuv_sub"
#define SENSOR_DRVNAME_HI556_MIPI_RAW_SUB       "hi556_mipi_raw_sub"
#define SENSOR_DRVNAME_HI2907_MIPI_RAW          "hi2907_mipi_raw"
/*MT*/
#define SENSOR_DRVNAME_MT9P012_RAW              "mt9p012_raw"
#define SENSOR_DRVNAME_MT9P015_RAW              "mt9p015_raw"
#define SENSOR_DRVNAME_MT9P017_RAW              "mt9p017_raw"
#define SENSOR_DRVNAME_MT9P017_MIPI_RAW         "mt9p017_mipi_raw"
#define SENSOR_DRVNAME_MT9D115_MIPI_RAW         "mt9d115_mipi_raw"
#define SENSOR_DRVNAME_MT9V114_YUV              "mt9v114_yuv"
#define SENSOR_DRVNAME_MT9V115_YUV              "mt9v115_yuv"
#define SENSOR_DRVNAME_MT9T113_YUV              "mt9t113_yuv"
#define SENSOR_DRVNAME_MT9V113_YUV              "mt9v113_yuv"
#define SENSOR_DRVNAME_MT9T113_MIPI_YUV         "mt9t113_mipi_yuv"
/*GC*/
#define SENSOR_DRVNAME_GC8054_MIPI_RAW          "gc8054_mipi_raw"
#define SENSOR_DRVNAME_GC5035MIPI_RAW           "gc5035_mipi_raw"
#define SENSOR_DRVNAME_GC2375_MIPI_RAW          "gc2375_mipi_raw"
#define SENSOR_DRVNAME_GC2375H_MIPI_RAW         "gc2375h_mipi_raw"
#define SENSOR_DRVNAME_GC2375SUB_MIPI_RAW       "gc2375sub_mipi_raw"
#define SENSOR_DRVNAME_GC2365_MIPI_RAW          "gc2365_mipi_raw"
#define SENSOR_DRVNAME_GC2366_MIPI_RAW          "gc2366_mipi_raw"
#define SENSOR_DRVNAME_GC2035_YUV               "gc2035_yuv"
#define SENSOR_DRVNAME_GC2235_RAW               "gc2235_raw"
#define SENSOR_DRVNAME_GC2355_MIPI_RAW          "gc2355_mipi_raw"
#define SENSOR_DRVNAME_GC2355_RAW               "gc2355_raw"
#define SENSOR_DRVNAME_GC0330_YUV               "gc0330_yuv"
#define SENSOR_DRVNAME_GC0329_YUV               "gc0329_yuv"
#define SENSOR_DRVNAME_GC2145_MIPI_YUV          "gc2145_mipi_yuv"
#define SENSOR_DRVNAME_GC2155MIPI_YUV           "gc2155_mipi_yuv"
#define SENSOR_DRVNAME_GC0310_MIPI_YUV          "gc0310_mipi_yuv"
#define SENSOR_DRVNAME_GC0310_YUV               "gc0310_yuv"
#define SENSOR_DRVNAME_GC0312_YUV               "gc0312_yuv"
#define SENSOR_DRVNAME_GC0313MIPI_YUV           "gc0313_mipi_yuv"
#define SENSOR_DRVNAME_GC0409_MIPI_RAW       	"gc0409_mipi_raw"
#define SENSOR_DRVNAME_GC5004_MIPI_RAW       	"gc5004_mipi_raw"
#define SENSOR_DRVNAME_GC5005_MIPI_RAW       	"gc5005_mipi_raw"
#define SENSOR_DRVNAME_GC5005_MIPI_RAW_SUB      "gc5005_mipi_raw_sub"
#define SENSOR_DRVNAME_GC5024_MIPI_RAW       	"gc5024_mipi_raw"
#define SENSOR_DRVNAME_GC8C34_MIPI_RAW          "gc8c34_mipi_raw"
#define SENSOR_DRVNAME_GC8024_MIPI_RAW			"gc8024_mipi_raw"
#define SENSOR_DRVNAME_GC8034_MIPI_RAW          "gc8034_mipi_raw"
#define SENSOR_DRVNAME_GC030AMIPI_RAW       	"gc030a_mipi_raw"
#define SENSOR_DRVNAME_GC030AMIPI_RAW_FRONT     "gc030a_mipi_raw_front"
#define SENSOR_DRVNAME_GC2235_MIPI_RAW          "gc2235_mipi_raw"
#define SENSOR_DRVNAME_GC2235_MIPI_RAW_SUB      "gc2235_mipi_raw_sub"
#define SENSOR_DRVNAME_GC5025_MIPI_RAW			"gc5025_mipi_raw"
#define SENSOR_DRVNAME_GC5025_MIPI_RAW_FRONT	"gc5025_mipi_raw_front"
#define SENSOR_DRVNAME_GC5035_MIPI_RAW			"gc5035_mipi_raw"
#define SENSOR_DRVNAME_GC5035_MIPI_RAW_SUB		"gc5035_mipi_raw_sub"
#define SENSOR_DRVNAME_GC5024MIPI_RAW           "gc5024_mipi_raw"
#define SENSOR_DRVNAME_GC8024_2LAN_MIPI_RAW     "gc8024_2lan_mipi_raw"
#define SENSOR_DRVNAME_GC2385_MIPI_RAW          "gc2385_mipi_raw"
#define SENSOR_DRVNAME_GC2385_MIPI_RAW_SUB      "gc2385_mipi_raw_sub"
#define SENSOR_DRVNAME_GC13023_MIPI_RAW         "gc13023_mipi_raw"
#define SENSOR_DRVNAME_GC033AMIPI_RAW			"gc033a_mipi_raw"
#define SENSOR_DRVNAME_GC8034MAIN_MIPI_RAW      "gc8034main_mipi_raw"
#define SENSOR_DRVNAME_GC6153_SERIAL_YUV         "gc6153_serial_yuv"
#define SENSOR_DRVNAME_GC08A3_MIPI_RAW          "gc08a3_mipi_raw"
/*SP*/
#define SENSOR_DRVNAME_SP0A19_YUV               "sp0a19_yuv"
#define SENSOR_DRVNAME_SP2518_YUV               "sp2518_yuv"
#define SENSOR_DRVNAME_SP2509_MIPI_RAW    		"sp2509_mipi_raw"
#define SENSOR_DRVNAME_SP2609_MIPI_RAW    		"sp2609_mipi_raw"
#define SENSOR_DRVNAME_SP2609_MIPI_RAW_SUB    	"sp2609-mipi_raw_sub"
#define SENSOR_DRVNAME_SP250A_MIPI_RAW    		"sp250a_mipi_raw"
#define SENSOR_DRVNAME_SP250A_MIPI_RAW_FRONT    "sp250a_mipi_rawf_ront"
#define SENSOR_DRVNAME_SP2509_MIPI_RAW_SUB    	"sp2509_mipi_raw_sub"
#define SENSOR_DRVNAME_SP2609_MIPI_RAW    		"sp2609_mipi_raw"
#define SENSOR_DRVNAME_SP0A09_MIPI_RAW    		"sp0a09_mipi_raw"
#define SENSOR_DRVNAME_SP0A20_YUV    			"sp0a20_yuv"
#define SENSOR_DRVNAME_SP5508_MIPI_RAW    		"sp5508_mipi_raw"
#define SENSOR_DRVNAME_SP2518_YUV               "sp2518_yuv"
#define SENSOR_DRVNAME_SP5409_MIPI_RAW    		"sp5409_mipi_raw"
#define SENSOR_DRVNAME_SP2508_MIPI_RAW    		"sp2508_mipi_raw"
#define SENSOR_DRVNAME_SP5507_MIPI_RAW    		"sp5507_mipi_raw"
#define SENSOR_DRVNAME_SP8408_MIPI_RAW    		"sp8408_mipi_raw"
#define SENSOR_DRVNAME_SP540A_MIPI_RAW    		"sp540a_mipi_raw"
#define SENSOR_DRVNAME_SP5506_MIPI_RAW         	"sp5506_mipi_raw"
/*A*/
#define SENSOR_DRVNAME_A5141_MIPI_RAW           "a5141_mipi_raw"
#define SENSOR_DRVNAME_A5142_MIPI_RAW           "a5142_mipi_raw"
/*HM*/
#define SENSOR_DRVNAME_HM3451_RAW               "hm3451_raw"
#define SENSOR_DRVNAME_HM5040_MIPI_RAW          "hm5040_mipi_raw"  //zxs 20150209
#define SENSOR_DRVNAME_HM8030_MIPI_RAW          "hm8030_mipi_raw"
#define SENSOR_DRVNAME_HM8040_MIPI_RAW          "hm8040_mipi_raw"
/*AR*/
#define SENSOR_DRVNAME_AR0543_MIPI_RAW          "ar0543_mipi_raw"
#define SENSOR_DRVNAME_AR0833_MIPI_RAW          "ar0833_mipi_raw"
#define SENSOR_DRVNAME_AR1335_MIPI_RAW          "ar1335_mipi_raw"
/*SIV*/
#define SENSOR_DRVNAME_SIV121D_YUV              "siv121d_yuv"
#define SENSOR_DRVNAME_SIV120B_YUV              "siv120b_yuv"
/*bf*/
#define SENSOR_DRVNAME_BF2206_MIPI_RAW    		"bf2206_mipi_raw"
#define SENSOR_DRVNAME_BF2206_MIPI_RAW_SUB   	"bf2206_mipi_raw_sub"
#define SENSOR_DRVNAME_BF20A1_MIPI_RAW    		"bf20a1_mipi_raw"
/*vd*/
#define SENSOR_DRVNAME_VD6955_MIPI_RAW    		"vd6955_mipi_raw"
/*PAS (PixArt Image)*/
#define SENSOR_DRVNAME_PAS6180_SERIAL_YUV       "pas6180_serial_yuv"
/*Panasoic*/
#define SENSOR_DRVNAME_MN34152_MIPI_RAW         "mn34152_mipi_raw"
#define SENSOR_DRVNAME_MN045_MIPI_RAW           "mn045_mipi_raw"
/*Toshiba*/
#define SENSOR_DRVNAME_T4KA7_MIPI_RAW           "t4ka7_mipi_raw"
#define SENSOR_DRVNAME_T4K37_MIPI_RAW           "t4k37_mipi_raw"
#define SENSOR_DRVNAME_T4KB3_MIPI_RAW		    "t4kb3_mipi_raw"
/*Others*/
#define SENSOR_DRVNAME_SHARP3D_MIPI_YUV         "sharp3d_mipi_yuv"
#define SENSOR_DRVNAME_T8EV5_YUV                "t8ev5_yuv"
#define SENSOR_DRVNAME_BF2253_MIPI_RAW          "bf2253_mipi_raw"
/*Test*/
#define SENSOR_DRVNAME_IMX135_MIPI_RAW_3MP "imx135_mipi_raw_3mp"
#define SENSOR_DRVNAME_IMX135_MIPI_RAW_5MP      "imx135_mipi_raw_5mp"
#define SENSOR_DRVNAME_IMX135_MIPI_RAW_8MP      "imx135_mipi_raw_8mp"
#define SENSOR_DRVNAME_OV13870_MIPI_RAW_5MP     "ov13870_mipi_raw_5mp"
#define SENSOR_DRVNAME_OV8856_MIPI_RAW_5MP      "ov8856_mipi_raw_5mp"
#define SENSOR_DRVNAME_IMX179_MIPI_RAW_5MP "imx179_mipi_raw_5mp"
#define SENSOR_DRVNAME_OV5670_MIPI_RAW_3MP "ov5670_mipi_raw_3mp"
#define SENSOR_DRVNAME_GC02M0_MIPI_RAW          "gc02m0_mipi_raw"
#define SENSOR_DRVNAME_OV02A10_MIPI_MONO        "ov02a10_mipi_mono"
#define SENSOR_DRVNAME_GC02M0B_MIPI_MONO        "gc02m0b_mipi_mono"
#define SENSOR_DRVNAME_GC02M0B_MIPI_MONO1       "gc02m0b_mipi_mono1"
#define SENSOR_DRVNAME_GC02M0B_MIPI_MONO2       "gc02m0b_mipi_mono2"
#define SENSOR_DRVNAME_GC02K0B_MIPI_MONO        "gc02k0b_mipi_mono"
#define SENSOR_DRVNAME_OV16A10_MIPI_RAW         "ov16a10_mipi_raw"
#define SENSOR_DRVNAME_GC02M1B_MIPI_MONO        "gc02m1b_mipi_mono"
/******************************************************************************
 *
 ******************************************************************************/
void KD_IMGSENSOR_PROFILE_INIT(void);
void KD_IMGSENSOR_PROFILE(char *tag);
void KD_IMGSENSOR_PROFILE_INIT_I2C(void);
void KD_IMGSENSOR_PROFILE_I2C(char *tag, int trans_num);

#define mDELAY(ms)     mdelay(ms)
#define uDELAY(us)       udelay(us)
#endif              /* _KD_IMGSENSOR_H */
