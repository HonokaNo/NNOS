#include "bootpack.h"
#include <string.h>

int little_endian(int big)
{
	char b0 = big & 0xff, b1 = (big >> 8) & 0xff;
	char b2 = (big >> 16) & 0xff, b3 = (big >> 24) & 0xff;
	return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

struct RSDT
{
	/* RSDT */
	char signature[4];
	unsigned int length;
	char revision;
	char checksum;
	char oem_id[6];
	char oem_table_id[8];
	char oem_revision[4];
	char creator_id[4];
	char creator_revision[4];
	unsigned int entry[];
};

struct FADT
{
	char signature[4];
	unsigned int length;
	unsigned char major_version;
	char checksum;
	char oem_id[6];
	char oem_table_id[8];
	char oem_revision[4];
	char craftor_id[4];
	char craftor_revision[4];
	unsigned int firmware_ctrl;	/* not used */
	unsigned int dsdt;
	char reserved;
	char preferred_pm_profile;
	short SCI_INT;
	int SMI_CMD;
	char ACPI_ENABLE;
	char ACPI_DISABLE;
	char S4BIOS_REQ;
	char PSTATE_CNT;
	int PM1a_EVT_BLK;
	int PM1b_EVT_BLK;
	int PM1a_CNT_BLK;
	int PM1b_CNT_BLK;
	int PM2_CNT_BLK;
	int PM_TMR_BLK;
	int GPE0_BLK;
	int GPE1_BLK;
	char PM1_EVT_LEN;
	char PM1_CNT_LEN;
	char PM2_CNT_LEN;
	char PM_TMR_LEN;
	char GPE0_BLK_LEN;
	char GPE1_BLK_LEN;
	char GPE1_BASE;
	char CST_CNT;
	short P_LVL2_LAT;
	short P_LVL3_LAT;
	short FLUSH_SIZE;
	short FLUSH_STRIDE;
	char DUTY_OFFSET;
	char DUTY_WIDTH;
	char DAY_ALRM;
	char MON_ALRM;
	char CENTURY;
	short IAPC_BOOT_ARCH;
	char reserved2;
	int FLAGS;
	/* 何かの構造体みたい よくわからない */
	char RESET_REG[12];
	char RESET_VALUE;
	short ARM_BOOT_ARCH;
	char FADT_MINOR_VERSION;
	/* 本来64bitアドレスなんだけどね... */
	/* 64bitサポートがないのでこうなっています */
	int X_FIRMWARE_CTRL[2];
	/* これも本来64bitアドレス */
	int X_DSDT[2];
	char X_PM1a_EVT_BLK[12];
	char X_PM1b_EVT_BLK[12];
	char X_PM1a_CNT_BLK[12];
	char X_PM1b_CNT_BLK[12];
	char X_PM2_CNT_BLK[12];
	char X_PM_TMR_BLK[12];
	char X_GPE0_BLK[12];
	char X_GPE1_BLK[12];
	char SLEEP_CONTROL_REG[12];
	char SLEEP_STATUS_REG[12];
	/* Hypervisor Vendor Identify */
	int HVI[2];
};

struct DSDT
{
	char signature[4];
	unsigned int length;
	char revision;
	char checksum;
	char oem_id[6];
	char oem_table_id[8];
	char oem_revision[4];
	char creator_id[4];
	char creator_revision[4];
	unsigned char *definition_block;
};

/* no return */
void acpi_hlt(struct BOOTINFO *binfo)
{
	int l, len;
	char *facp = 0, *buf, s[5];

	/* ACPI実装確認 */
	if(binfo->acpi != 0){
		struct RSDT *rsdt = (struct RSDT *)(binfo->acpi);

		len = (rsdt->length - 36) / 4;
		if(!memcmp(rsdt->signature, "RSDT", 4)){
			printlog("RSDT header\n");
			for(l = 0; l < len; l++){
				buf = (char *)rsdt->entry[l];
				memcpy(s, buf, 4);
				s[4] = 0;
				if(!strncmp(buf, "FACP", 4)){
					facp = buf;
					printlog("FADT table\n");
				}
			}
		}

		if(facp != 0){
			struct FADT *fadt = (struct FADT *)facp;

			struct DSDT *dsdt = (struct DSDT *)fadt->dsdt;
			if(dsdt != 0){
				len = (dsdt->length - 36);

				printlog("DSDT:%d\n", dsdt);
			}
		}
	}

	return;
}
