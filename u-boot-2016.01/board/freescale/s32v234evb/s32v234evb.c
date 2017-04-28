/*
 * (C) Copyright 2013-2016 Freescale Semiconductor, Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/io.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/siul.h>
#include <asm/arch/clock.h>
#include <asm/arch/xrdc.h>
#include <fdt_support.h>
#include <libfdt.h>
#include <miiphy.h>
#include <netdev.h>
#include <i2c.h>

DECLARE_GLOBAL_DATA_PTR;

static void setup_iomux_uart(void)
{
	/* Muxing for linflex0 and linflex1 */

	/* set PA12 - MSCR[12] - for UART0 TXD */
	writel(SIUL2_MSCR_PORT_CTRL_UART_TXD, SIUL2_MSCRn(SIUL2_MSCR_PA12));

	/* set PA11 - MSCR[11] - for UART0 RXD */
	writel(SIUL2_MSCR_PORT_CTRL_UART_RXD, SIUL2_MSCRn(SIUL2_MSCR_PA11));
	/* set UART0 RXD - IMCR[200] - to link to PA11 */
	writel(SIUL2_IMCR_UART_RXD_to_pad, SIUL2_IMCRn(SIUL2_IMCR_UART0_RXD));

	/* set PA14 - MSCR[14] - for UART1 TXD*/
	writel(SIUL2_MSCR_PORT_CTRL_UART_TXD, SIUL2_MSCRn(SIUL2_MSCR_PA14));

	/* set PA13 - MSCR[13] - for UART1 RXD */
	writel(SIUL2_MSCR_PORT_CTRL_UART_RXD, SIUL2_MSCRn(SIUL2_MSCR_PA13));
	/* set UART1 RXD - IMCR[202] - to link to PA13 */
	writel(SIUL2_IMCR_UART_RXD_to_pad, SIUL2_IMCRn(SIUL2_IMCR_UART1_RXD));
}

static void setup_iomux_enet(void)
{
#ifndef CONFIG_PHY_RGMII_DIRECT_CONNECTED
	writel(0x0020c701, SIUL2_MSCRn(45));	//MDC   //PC13
	writel(0x0028c701, SIUL2_MSCRn(46));	//MDIO  //PC14
	writel(       0x2, SIUL2_MSCRn(981));
#endif

#ifdef CONFIG_PHY_RGMII_DIRECT_CONNECTED
	writel(0x0020c701, SIUL2_MSCRn(47));	//TX_CLK //PC15
#else
	writel(0x00203701, SIUL2_MSCRn(47));	//TX_CLK //PC15
#endif
	writel(       0x2, SIUL2_MSCRn(978));

	writel(0x0008c700, SIUL2_MSCRn(48));	//RX_CLK //PD0
	writel(       0x2, SIUL2_MSCRn(979));

	writel(0x0008c700, SIUL2_MSCRn(49));	//RX_D0  //PD1
	writel(       0x2, SIUL2_MSCRn(974));

	writel(0x0008c700, SIUL2_MSCRn(50));	//RX_D1  //PD2
	writel(       0x2, SIUL2_MSCRn(975));


	writel(0x0008c700, SIUL2_MSCRn(51));	//RX_D2  //PD3
	writel(       0x2, SIUL2_MSCRn(976));

	writel(0x0008c700, SIUL2_MSCRn(52));	//RX_D3  //PD4
	writel(       0x2, SIUL2_MSCRn(977));

	writel(0x0008c700, SIUL2_MSCRn(53));	//RX_DV  //PD5
	writel(       0x2, SIUL2_MSCRn(973));

	writel(0x0020c701, SIUL2_MSCRn(55));	//TX_D0  //PD7
	writel(0x0020c701, SIUL2_MSCRn(56));	//TX_D1  //PD8
	writel(0x0020c701, SIUL2_MSCRn(57));	//TX_D2  //PD9
	writel(0x0020c701, SIUL2_MSCRn(58));	//TX_D3  //PD10
	writel(0x0020c701, SIUL2_MSCRn(59));	//TX_EN  //PD11

#if 0
	/* reset the Ethernet controller */
	writel(0x1, 0x40032024);
	while (readl(0x40032024) & 0x1);
#endif
}

static void setup_iomux_i2c(void)
{
	/* I2C0 - Serial Data Input */
	writel(SIUL2_PAD_CTRL_I2C0_MSCR_SDA, SIUL2_MSCRn(99));
	writel(SIUL2_PAD_CTRL_I2C0_IMCR_SDA, SIUL2_IMCRn(269));

	/* I2C0 - Serial Clock Input */
	writel(SIUL2_PAD_CTRL_I2C0_MSCR_SCLK, SIUL2_MSCRn(100));
	writel(SIUL2_PAD_CTRL_I2C0_IMCR_SCLK, SIUL2_IMCRn(268));

	/* I2C1 - Serial Data Input */
	writel(SIUL2_PAD_CTRL_I2C1_MSCR_SDA, SIUL2_MSCRn(101));
	writel(SIUL2_PAD_CTRL_I2C1_IMCR_SDA, SIUL2_IMCRn(271));

	/* I2C1 - Serial Clock Input */
	writel(SIUL2_PAD_CTRL_I2C1_MSCR_SCLK, SIUL2_MSCRn(102));
	writel(SIUL2_PAD_CTRL_I2C1_IMCR_SCLK, SIUL2_IMCRn(270));

	/* I2C2 - Serial Data Input */
	writel(SIUL2_PAD_CTRL_I2C2_MSCR_SDA, SIUL2_MSCRn(19));
	writel(SIUL2_PAD_CTRL_I2C2_IMCR_SDA, SIUL2_IMCRn(273));

	/* I2C2 - Serial Clock Input */
	writel(SIUL2_PAD_CTRL_I2C2_MSCR_SCLK, SIUL2_MSCRn(20));
	writel(SIUL2_PAD_CTRL_I2C2_IMCR_SCLK, SIUL2_IMCRn(272));
}

static void setup_iomux_spi2(void)
{
        /* SPI2 - Chip Select */
        writel(0x00200700, SIUL2_MSCRn(32));
        
        /* SPI2 - Serial Clock Output */
        writel(0x00200401, SIUL2_MSCRn(29));

        /* SPI2 - Serial Data Output*/
        writel(0x00200401, SIUL2_MSCRn(30));

        /* SPI2 - Serial Data Input */
        writel(0x00080000, SIUL2_MSCRn(31));
        writel(0x00000002, SIUL2_IMCRn(294));
}

#ifdef CONFIG_SYS_USE_NAND
void setup_iomux_nfc(void)
{
	/*TODO: Implement nfc iomux when it is activated.*/
}
#endif

static void mscm_init(void)
{
	struct mscm_ir *mscmir = (struct mscm_ir *)MSCM_BASE_ADDR;
	int i;

	for (i = 0; i < MSCM_IRSPRC_NUM; i++)
		writew(MSCM_IRSPRC_CPn_EN, &mscmir->irsprc[i]);
}

int board_phy_config(struct phy_device *phydev)
{
	if (phydev->drv->config)
		phydev->drv->config(phydev);

	return 0;
}
#define SPI2_MCR     0x800f3000
#define SPI2_CTAR0   0x38000005
#define SPI2_SR      (SPI2_BASE_ADDR + 0x2c)
#define SPI2_PUSHR   (SPI2_BASE_ADDR + 0x34)
#define SPI2_POPR    (SPI2_BASE_ADDR + 0x38)
#define SPI2_GPIO    (SIUL2_BASE_ADDR + 0x1320)

static int cs_low(void)
{
	__raw_writel(SPI2_GPIO,(__raw_readl(SPI2_GPIO) & (~0x01000000U)));
}
static int cs_high(void)
{
	__raw_writel(SPI2_GPIO,(__raw_readl(SPI2_GPIO) | (~0xFEFFFFFFU)));
}

static check_stat(void)
{
        while ( ((__raw_readl(SPI2_SR)) & (0x80000000U)) == 0);
        __raw_writel(SPI2_SR,(__raw_readl(SPI2_SR) | (0x800a0000)));
        while ( ((__raw_readl(SPI2_SR)) & (0x00020000U)) == 0);
        __raw_writel(SPI2_SR,(__raw_readl(SPI2_SR) | (0x000a0000)));
}

static void setup_mii_enet_switch(void)
{
	cs_low();
	/* Set TCF, RFDF, RFOF to 1 */
	__raw_writel(SPI2_SR,(__raw_readl(SPI2_SR) | (0x800a0000)));
	/* Tx 0x03 */
	__raw_writel(SPI2_PUSHR,0x00000003);
	check_stat();

	__raw_writel(SPI2_PUSHR,0x00000000);
	check_stat();

	__raw_writel(SPI2_PUSHR,0x00000000);
	check_stat();

	cs_high();
	cs_low();
		
 	__raw_writel(SPI2_SR,(__raw_readl(SPI2_SR) | (0x800a0000)));
	
	__raw_writel(SPI2_PUSHR,0x00000002);
	check_stat();

	__raw_writel(SPI2_PUSHR,0x00000001);
	check_stat();

	__raw_writel(SPI2_PUSHR,0x00000041);
	check_stat();
       
	cs_high();
	cs_low();
 	
	__raw_writel(SPI2_SR,(__raw_readl(SPI2_SR) | (0x800a0000)));
	
	__raw_writel(SPI2_PUSHR,0x00000003);
	check_stat();

	__raw_writel(SPI2_PUSHR,0x00000001);
	check_stat();
	
	__raw_writel(SPI2_PUSHR,0x00000000);
	check_stat();
	cs_high();
}

void setup_xrdc(void)
{
	writel(XRDC_ADDR_MIN, XRDC_MRGD_W0_16);
	writel(XRDC_ADDR_MAX, XRDC_MRGD_W1_16);
	writel(XRDC_VALID, XRDC_MRGD_W3_16);

	writel(XRDC_ADDR_MIN, XRDC_MRGD_W0_17);
	writel(XRDC_ADDR_MAX, XRDC_MRGD_W1_17);
	writel(XRDC_VALID, XRDC_MRGD_W3_17);

	writel(XRDC_ADDR_MIN, XRDC_MRGD_W0_18);
	writel(XRDC_ADDR_MAX, XRDC_MRGD_W1_18);
	writel(XRDC_VALID, XRDC_MRGD_W3_18);

	writel(XRDC_ADDR_MIN, XRDC_MRGD_W0_19);
	writel(XRDC_ADDR_MAX, XRDC_MRGD_W1_19);
	writel(XRDC_VALID, XRDC_MRGD_W3_19);
}

#ifdef CONFIG_DCU_QOS_FIX
int board_dcu_qos(void)
{
	writel(0x0, 0x40012380);
	writel(0x0, 0x40012384);
	writel(0x0, 0x40012480);
	writel(0x0, 0x40012484);
	writel(0x0, 0x40012580);
	writel(0x0, 0x40012584);
	writel(0x0, 0x40012680);
	writel(0x0, 0x40012684);
	writel(0x0, 0x40012780);
	writel(0x0, 0x40012784);
	writel(0x0, 0x40012880);
	writel(0x0, 0x40012884);
	writel(0x0, 0x40012980);
	writel(0x0, 0x40012984);
	writel(0x0, 0x40012A80);
	writel(0x0, 0x40012A84);
	writel(0x0, 0x40012B80);
	writel(0x0, 0x40012B84);
	writel(0x0, 0x40012C80);
	writel(0x0, 0x40012C84);
	writel(0x0, 0x40012D80);
	writel(0x0, 0x40012D84);
	writel(0x0, 0x40012E80);
	writel(0x0, 0x40012E84);
	writel(0x0, 0x40012F80);
	writel(0x0, 0x40012F84);

	return 0;
}
#endif

int board_early_init_f(void)
{
	clock_init();
	mscm_init();

	setup_iomux_uart();
	setup_iomux_enet();
	setup_iomux_i2c();
#ifdef CONFIG_SYS_USE_NAND
	setup_iomux_nfc();
#endif
        setup_iomux_spi2();

#ifdef CONFIG_DCU_QOS_FIX
	board_dcu_qos();
#endif
        setup_mii_enet_switch();
	setup_xrdc();
	return 0;
}

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM + 0x100;

	return 0;
}

int checkboard(void)
{
	puts("Board: s32v234evb\n");

	return 0;
}


#if defined(CONFIG_OF_FDT) && defined(CONFIG_OF_BOARD_SETUP)
int ft_board_setup(void *blob, bd_t *bd)
{
	ft_cpu_setup(blob, bd);
	return 0;
}
#endif /* defined(CONFIG_OF_FDT) && defined(CONFIG_OF_BOARD_SETUP) */
