#include "bootpack.h"
#include <string.h>

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
	/* DSDT */
	char signature[4];
	unsigned int length;
	char revision;
	char checksum;
	char oem_id[6];
	char oem_table_id[8];
	char oem_revision[4];
	char creator_id[4];
	char creator_revision[4];
	unsigned char definition_block[];
};

/* no return */
void acpi_hlt(struct BOOTINFO *binfo, int *fat)
{
	int l, len;
	char *facp = 0, *buf, s[5];
	unsigned char *_s5_ = 0;

	/* ACPI実装確認 */
	if(binfo->acpi != 0){
		struct RSDT *rsdt = (struct RSDT *)(binfo->acpi);

		while(memcmp(rsdt->signature, "RSDT", 4)) rsdt++;

		len = (rsdt->length - 36) / 4;
		if(!memcmp(rsdt->signature, "RSDT", 4)){
			for(l = 0; l < len; l++){
				buf = (char *)rsdt->entry[l];
				memcpy(s, buf, 4);
				s[4] = 0;
<<<<<<< HEAD
				if(!strncmp(buf, "FACP", 4)) facp = buf;
=======
				if(!strncmp(buf, "FACP", 4)){
					facp = buf;
				}
>>>>>>> developer
			}
		}

		if(facp != 0){
			struct FADT *fadt = (struct FADT *)facp;

			struct DSDT *dsdt = (struct DSDT *)fadt->dsdt;
			if(!strncmp((char *)dsdt, "DSDT", 4)){
				len = (dsdt->length - 36);

				/* データを表すには最低でも11バイトは必要 */
				for(l = 0; l < len; l++){
					memcpy(s, &dsdt->definition_block[l], 4);
					s[4] = 0;
					if(!strncmp(s, "_S5_", 4)){
						_s5_ = (char *)&dsdt->definition_block[l];
						break;
					}
				}

				if(_s5_ != 0){
					/* skip "_S5_" */
					_s5_ += 4;

					/* どちらも1-255 */
					char s5_len = _s5_[1];
					char s5_elements = _s5_[2];
					char pm1a_set = 0, pm1b_set = 0;

					/* すべて定数オブジェクトで1バイトずつ格納 */
					if(s5_len == 6 && s5_elements == 4){
						pm1a_set = _s5_[3];
						pm1b_set = _s5_[4];

						if(fadt->PM1a_CNT_BLK != 0){
							short set_pm1a = pm1a_set;
							set_pm1a <<= 10;
							set_pm1a |= (1 << 13);
							io_out16(fadt->PM1a_CNT_BLK, set_pm1a);

							if(fadt->PM1b_CNT_BLK != 0){
								short set_pm1b = pm1b_set;
								set_pm1b <<= 10;
								set_pm1b |= (1 << 13);
								io_out16(fadt->PM1b_CNT_BLK, set_pm1b);
							}
						}
					/* ダブルワード */
					}else if(s5_len == 7 && s5_elements == 1){
						if(_s5_[3] == 0x0c){
							pm1a_set = _s5_[7];
							pm1b_set = _s5_[6];
						}

						if(fadt->PM1a_CNT_BLK != 0){
							short set_pm1a = pm1a_set;
							set_pm1a <<= 10;
							set_pm1a |= (1 << 13);
							io_out16(fadt->PM1a_CNT_BLK, set_pm1a);

							if(fadt->PM1b_CNT_BLK != 0){
								short set_pm1b = pm1b_set;
								set_pm1b <<= 10;
								set_pm1b |= (1 << 13);
								io_out16(fadt->PM1b_CNT_BLK, set_pm1b);
							}
						}
					}else{
						/* いらないデータは省く */
						_s5_ += 3;
						if(_s5_[0] == 0x00 || _s5_[0] == 0x01 || _s5_[0] == 0xff){
							pm1a_set = _s5_[0];
						}else if(_s5_[0] == 0x0a){
							pm1a_set  = _s5_[1];
							_s5_++;
						}
						_s5_++;
						if(_s5_[0] == 0x00 || _s5_[0] == 0x01 || _s5_[0] == 0xff){
							pm1b_set = _s5_[0];
						}else if(_s5_[0] == 0x0a){
							pm1b_set  = _s5_[1];
						}

						if(fadt->PM1a_CNT_BLK != 0){
							short set_pm1a = pm1a_set;
							set_pm1a <<= 10;
							set_pm1a |= (1 << 13);
							io_out16(fadt->PM1a_CNT_BLK, set_pm1a);

							if(fadt->PM1b_CNT_BLK != 0){
								short set_pm1b = pm1b_set;
								set_pm1b <<= 10;
								set_pm1b |= (1 << 13);
								io_out16(fadt->PM1b_CNT_BLK, set_pm1b);
							}
						}
					}
				}
			}
		}
	}

	return;
}
