/* -----------------------------------------------------------------------------
 * Copyright (c) 2014 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        21. July 2014
 * $Revision:    V1.00
 *
 * Project:      Flash Programming Functions for ST STM32L4xx Flash
 * --------------------------------------------------------------------------- */

/* History:
 *  Version 1.00
 *    Initial release
 */

#include "..\FlashOS.H"        // FlashOS Structures

typedef volatile unsigned char    vu8;
typedef          unsigned char     u8;
typedef volatile unsigned short   vu16;
typedef          unsigned short    u16;
typedef volatile unsigned long    vu32;
typedef          unsigned long     u32;
typedef volatile unsigned long    vu64;
typedef          unsigned long     u64;

static u32 bank1_addr=0x08040000;
//static u32 DUALBANK=0;

//static u32 PWR_CR1_RESTORE;

#define M8(adr)  (*((vu8  *) (adr)))
#define M16(adr) (*((vu16 *) (adr)))
#define M32(adr) (*((vu32 *) (adr)))
#define M64(adr) (*((vu64 *) (adr)))
#define FLASH_CR_BKER       ((u32)0x00000800)

// Peripheral Memory Map
#define IWDG_BASE         0x40003000
#define FLASH_BASE        0x40022000
#define PWR_BASE        0x40007000
#define RCC_BASE        0x40021000
//0x40023C00

#define IWDG            ((IWDG_TypeDef *) IWDG_BASE)
#define FLASH           ((FLASH_TypeDef*) FLASH_BASE)
#define PWR           ((PWR_TypeDef*) PWR_BASE)
#define RCC           ((RCC_TypeDef*) RCC_BASE)

// RCC
typedef struct {
  vu32 CR;
} RCC_TypeDef;


// Independent WATCHDOG
typedef struct {
  vu32 KR;
  vu32 PR;
  vu32 RLR;
  vu32 SR;
} IWDG_TypeDef;

// Flash Registers
typedef struct {
  vu32 ACR;           
  vu32 PDKEYR;				
  vu32 KEYR;					
  vu32 OPTKEYR;				
  vu32 SR;						
  vu32 CR;									
  vu32 ECCR;					
  vu32 RESERVED0;			
  vu32 OPTR;									
} FLASH_TypeDef;

// PWR Registers

typedef struct
{
  vu32 CR1;   /*!< PWR power control register 1,        Address offset: 0x00 */
  vu32 CR2;   /*!< PWR power control register 2,        Address offset: 0x04 */
  vu32 CR3;   /*!< PWR power control register 3,        Address offset: 0x08 */
  vu32 CR4;   /*!< PWR power control register 4,        Address offset: 0x0C */
  vu32 SR1;   /*!< PWR power status register 1,         Address offset: 0x10 */
  vu32 SR2;   /*!< PWR power status register 2,         Address offset: 0x14 */
  vu32 SCR;   /*!< PWR power status reset register,     Address offset: 0x18 */
  vu32 RESERVED;   /*!< Reserved,                            Address offset: 0x1C */
  vu32 PUCRA; /*!< Pull_up control register of portA,   Address offset: 0x20 */
  vu32 PDCRA; /*!< Pull_Down control register of portA, Address offset: 0x24 */
  vu32 PUCRB; /*!< Pull_up control register of portB,   Address offset: 0x28 */
  vu32 PDCRB; /*!< Pull_Down control register of portB, Address offset: 0x2C */
  vu32 PUCRC; /*!< Pull_up control register of portC,   Address offset: 0x30 */
  vu32 PDCRC; /*!< Pull_Down control register of portC, Address offset: 0x34 */
  vu32 PUCRD; /*!< Pull_up control register of portD,   Address offset: 0x38 */
  vu32 PDCRD; /*!< Pull_Down control register of portD, Address offset: 0x3C */
  vu32 PUCRE; /*!< Pull_up control register of portE,   Address offset: 0x40 */
  vu32 PDCRE; /*!< Pull_Down control register of portE, Address offset: 0x44 */
  vu32 PUCRF; /*!< Pull_up control register of portF,   Address offset: 0x48 */
  vu32 PDCRF; /*!< Pull_Down control register of portF, Address offset: 0x4C */
  vu32 PUCRG; /*!< Pull_up control register of portG,   Address offset: 0x50 */
  vu32 PDCRG; /*!< Pull_Down control register of portG, Address offset: 0x54 */
  vu32 PUCRH; /*!< Pull_up control register of portH,   Address offset: 0x58 */
  vu32 PDCRH; /*!< Pull_Down control register of portH, Address offset: 0x5C */
} PWR_TypeDef;


// Flash Keys
#define RDPRT_KEY                0x00A5
#define FLASH_KEY1               0x45670123   
#define FLASH_KEY2               0xCDEF89AB
#define FLASH_OPTKEY1            0x08192A3B
#define FLASH_OPTKEY2            0x4C5D6E7F

// Flash Control Register definitions
#define FLASH_PG                ((unsigned int)(1U ))
#define FLASH_PER               ((unsigned int)(1U <<  1))
#define FLASH_MER1              ((unsigned int)(1U <<  2))
#define FLASH_PNB_MSK           ((unsigned int)(0x7F << 3))
#define FLASH_BKER              ((unsigned int)(1U << 11))
#define FLASH_MER2              ((unsigned int)(1U << 15))
#define FLASH_STRT              ((unsigned int)(1U << 16))
#define FLASH_LOCK              ((unsigned int)(1U << 31))
#define FLASH_FSTPG              ((unsigned int)(1U << 18))

// Flash Status Register definitions
#define FLASH_EOP               ((unsigned int)(1U <<  0))
#define FLASH_OPERR             ((unsigned int)(1U <<  1))
#define FLASH_PROGERR           ((unsigned int)(1U <<  3))
#define FLASH_WRPERR            ((unsigned int)(1U <<  4))
#define FLASH_PGAERR            ((unsigned int)(1U <<  5))
#define FLASH_SIZERR            ((unsigned int)(1U <<  6))
#define FLASH_PGSERR            ((unsigned int)(1U <<  7))
#define FLASH_MISSERR           ((unsigned int)(1U <<  8))
#define FLASH_FASTERR           ((unsigned int)(1U <<  9))
#define FLASH_RDERR             ((unsigned int)(1U << 14))
#define FLASH_OPTR_DUALBANK1   ((u32)0x00400000)

// Flash ACR Register definitions
#define FLASH_ACR_DCEN           ((unsigned int)(1U <<  10))
#define FLASH_ACR_ICEN           ((unsigned int)(1U <<  9))


#define RCC_HSION   						((unsigned int)(1U <<  8))
#define RCC_MSION               ((unsigned int)(1U <<  0))

#define FLASH_BSY               ((unsigned int)(1U << 16))
	
#define VddLow                 ((unsigned int)(1U << 23))

#define FLASH_PGERR             (FLASH_OPERR  | FLASH_PROGERR | FLASH_PROGERR | FLASH_WRPERR  | FLASH_PGAERR | \
                                 FLASH_SIZERR | FLASH_PGSERR  | FLASH_MISSERR | FLASH_FASTERR | FLASH_RDERR   )

//#if   defined STM32L4x_1024      /* 2 * 512 kB */
//  #define FLASH_BANK_SIZE       (0x08080000U)
//#elif defined STM32L4x_2048       /* 2 * 1024 kB  or 1 * 2048 kB*/
 // #define FLASH_BANK_SIZE       (0x08100000U)
//#elif defined STM32L4x_512       /* 2 * 256 kB  or 1 * 512 kB*/
//  #define FLASH_BANK_SIZE       (0x08040000U)
//#elif defined STM32L4x_256       /* 2 * 128 kB  or 1 * 256 kB*/
//  #define FLASH_BANK_SIZE       (0x08020000U)
//#else
//  #error STM32L4xx Flash size defined nort defined!
//#endif


//unsigned long GetBankNum(unsigned long adr) {
//  unsigned long bank;
  
//  if ((FLASH->OPTR & (1U << 21)) == (1U << 21)) {
    /* Dual-Bank Flash */
//    if (adr >= FLASH_BANK_SIZE) {
//      bank = 1u;           /* FLASH_CR.BKER (bit11) 1: Bank 2 is selected for page erase */
 //   }
//    else {
//      bank = 0u;           /* FLASH_CR.BKER (bit11) 0: Bank 1 is selected for page erase */
//    }
//  } 
  /* Single-Bank Flash */
//  else {
//    bank = 0u;             /* FLASH_CR.BKER (bit11) 0: Bank 1 is selected for page erase */
//  }
  
//  return (bank);
//}


/*
 *  Initialize Flash Programming Functions
 *    Parameter:      adr:  Device Base Address
 *                    clk:  Clock Frequency (Hz)
 *                    fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */


int Init (unsigned long adr, unsigned long clk, unsigned long fnc) {

  FLASH->KEYR = FLASH_KEY1;                             // Unlock Flash
  FLASH->KEYR = FLASH_KEY2;

 // FLASH->ACR  = 0x00000005;                             // 5 Wait State, no Cache, no Prefetch
//	FLASH->ACR  &= (~FLASH_ACR_DCEN); 
//	FLASH->ACR  &= (~FLASH_ACR_ICEN); 
	
 // FLASH->SR   = FLASH_PGERR;                            // Reset Error Flags

  /*Save thePWR_CR1 register
    to restore it later*/
//  PWR_CR1_RESTORE = PWR->CR1;

	  /*change voltage range to range1*/
  
//  PWR->CR1 |= 0x00000200; 
//  PWR->CR1 &= 0xFFFFFBFF; 
	
//	 if((FLASH->OPTR & FLASH_OPTR_DUALBANK1))
//  {/*Dual Bank devices*/
//   bank1_addr = 0x08100000;
//    DUALBANK=1;
    
 // }

//  if ((FLASH->OPTR & 0x10000) == 0x00000) {             // Test if IWDG is running (IWDG in HW mode)
    // Set IWDG time out to ~32.768 second
//    IWDG->KR  = 0x5555;                                 // Enable write access to IWDG_PR and IWDG_RLR
//    IWDG->PR  = 0x06;                                   // Set prescaler to 256
//    IWDG->RLR = 4095;                                   // Set reload value to 4095
//  }

     /*Wait until the flash is ready*/
  while (FLASH->SR & FLASH_BSY);
	
//	ProgramPage(0x08000000,4 , (unsigned char*)0x20000000);


////FLASH->CR |= FLASH_PG ;
////M32(0x8040000) = 0xAAAAAAAA;
////M32(0x8040004) = 0xBBBBBBBB;		
////FLASH->CR &= ~FLASH_PG;		


  return (0);
}



/*
 *  De-Initialize Flash Programming Functions
 *    Parameter:      fnc:  Function Code (1 - Erase, 2 - Program, 3 - Verify)
 *    Return Value:   0 - OK,  1 - Failed
 */

int UnInit (unsigned long fnc) {

  FLASH->CR |= FLASH_LOCK;                                // Lock Flash

  return (0);
}


/*  
 *  Blank Check Checks if Memory is Blank
 *    Parameter:      adr:  Block Start Address
 *                    sz:   Block Size (in bytes)
 *                    pat:  Block Pattern
 *    Return Value:   0 - OK,  1 - Failed
 */

int BlankCheck (unsigned long adr, unsigned long sz, unsigned char pat) {
  /* force erase even if the content is 'Initial Content of Erased Memory'.
     Only a erased sector can be programmed. I think this is because of ECC */
  return (1);
}

/*
 *  Erase complete Flash Memory
 *    Return Value:   0 - OK,  1 - Failed
 */

int EraseChip (void) {

  while (FLASH->SR & FLASH_BSY)	

  FLASH->SR = 0x0000C3FB;                               // Reset Error Flags
  
  FLASH->CR  = (FLASH_MER1 | FLASH_MER2);                 // Bank A/B Mass Erase Enabled
  FLASH->CR |=  FLASH_STRT;                               // Start Erase
  
  while (FLASH->SR & FLASH_BSY) {
  //  IWDG->KR = 0xAAAA;                                    // Reload IWDG
  }

  //FLASH->CR  =  0;                                        // Reset CR

  if (FLASH->SR & FLASH_PGERR) {                          // Check for Error
    FLASH->SR  = FLASH_PGERR;                             // Reset Error Flags
    return (1);                                           // Failed
  }
	
		FLASH->CR &= (~FLASH_MER1);
		FLASH->CR &= (~FLASH_MER2);

  return (0);                                             
}


/*
 *  Erase Sector in Flash Memory
 *    Parameter:      adr:  Sector Address
 *    Return Value:   0 - OK,  1 - Failed
 */

#ifdef DualBank

int EraseSector (unsigned long adr) {
  unsigned long page;
	
	 /*Clear the error status*/
   //   FLASH->SR = 0x0000C3FB;
	
	      /*page erase enabled*/ 
        FLASH->CR |= FLASH_PER;
	
	    /*Calculate the page */  
         /*bank 2 */  
      if(adr >= bank1_addr)
         {
           page = (adr >>11);
           page = page - 0x80;
           page = page & 0x0000007F;
           
           FLASH->CR |= (FLASH_CR_BKER);           
         }
        /*bank 1 */     
      else
        {
           page = ((adr >>11) & 0x0000007F );

           FLASH->CR &= ~(FLASH_CR_BKER);           
         }
				
				 
				 
		      /*set PNB*/
       FLASH->CR &= ~(FLASH_PNB_MSK);

       
       FLASH->CR |= page << 3;
  
    /*Start the erase operation*/  
      FLASH->CR |= FLASH_STRT;		 
				 
				 
				 
	

  while (FLASH->SR & FLASH_BSY) {
  //  IWDG->KR = 0xAAAA;                                    // Reload IWDG
  }



	  /*wait until the operation ends*/
    while (FLASH->SR & FLASH_BSY);
	
  if (FLASH->SR & FLASH_PGERR) {                          // Check for Error
    FLASH->SR  = FLASH_PGERR;                             // Reset Error Flags
    return (1);                                           // Failed
  }

	  FLASH->CR &= (~FLASH_PER);

	
  return (0);                                            
}

#endif


#ifdef SingleBank

int EraseSector (unsigned long adr) {
  unsigned long page;

	
	      /*Clear the error status*/
   //   FLASH->SR = 0x0000C3FB;
	
	      /*page erase enabled*/ 
        FLASH->CR |= FLASH_PER;
	
	    /*Calculate the page */  
         /*bank 2 */  

        /*bank 1 */     

           page = ((adr >>12) & 0x0000007F );

           FLASH->CR &= ~(FLASH_CR_BKER);    
         
			 
		      /*set PNB*/
       FLASH->CR &= ~(FLASH_PNB_MSK);

       
       FLASH->CR |= page << 3;
  
    /*Start the erase operation*/  
      FLASH->CR |= FLASH_STRT;		

  while (FLASH->SR & FLASH_BSY) {
  //  IWDG->KR = 0xAAAA;                                    // Reload IWDG
  }



	  /*wait until the operation ends*/
    while (FLASH->SR & FLASH_BSY);
	
  if (FLASH->SR & FLASH_PGERR) {                          // Check for Error
    FLASH->SR  = FLASH_PGERR;                             // Reset Error Flags
    return (1);                                           // Failed
  }
	  FLASH->CR &= (~FLASH_PER);	
  return (0);                                             
}

#endif

/*
 *  Program Page in Flash Memory
 *    Parameter:      adr:  Page Start Address
 *                    sz:   Page Size
 *                    buf:  Page Data
 *    Return Value:   0 - OK,  1 - Failed
 */

int ProgramPage (unsigned long adr, unsigned long sz, unsigned char *buf) {

      sz = (sz + 7) & ~7;                                     // Adjust size for two words

//	 FLASH->ACR = 0x02000600;
	
	
    while (FLASH->SR & FLASH_BSY) {
   // IWDG->KR = 0xAAAA;                                    // Reload IWDG
    }

   FLASH->SR = 0x0000C3FB;                                // Reset Error Flags
//		FLASH->SR = 0x0000C3FD;                                // Reset Error Flags
		
		FLASH->CR |= FLASH_PG ;	                              // Programming Enabled
////      M32(0x08000000) = 0xAAAAAAAA;                    
////      M32(0x08000004) = 0xBBBBBBBB; 	
////		FLASH->CR &= ~FLASH_PG;	
		
		 
  while (sz) {

		//				FLASH->CR |= FLASH_PG ;	
      M32(adr    ) = *((u32 *)(buf + 0));                   // Program the first word of the Double Word
      M32(adr + 4) = *((u32 *)(buf + 4));                   // Program the second word of the Double Word
		// while (FLASH->SR & FLASH_BSY);
	 //	M32(0x08000000) = 0xAAAAAAAA;  
 	
		// M32(0x08000004) = 0xBBBBBBBB;
   // while (FLASH->SR & FLASH_BSY)	;
	//	 M32(0x08000008) = 0x11111111;  
	//	M32(0x0800000C) = 0x55555555;
      while (FLASH->SR & FLASH_BSY) {
       // IWDG->KR = 0xAAAA;                                  // Reload IWDG
     }
    
     // FLASH->CR  =  0;                                      // Reset CR
    adr += 8;                                             // Go to next DoubleWord
    buf += 8;
    sz  -= 8;
		
////		    while (FLASH->SR & FLASH_BSY) {
////   // IWDG->KR = 0xAAAA;                                    // Reload IWDG
////    }
				
			//	FLASH->CR &= ~FLASH_PG;	
		
      if ((FLASH->SR & FLASH_EOP)) {                        // Check for Error
        FLASH->SR  = FLASH_PGERR;                           // Reset Error Flags
        return (1);                                         // Failed
      }
		
    //  FLASH->CR &= ~FLASH_PG ;	                                 // Reset CR
		

  }
		FLASH->CR &= ~FLASH_PG;	

	
	  /*Restore PWR_CR1*/
//  PWR->CR1 = PWR_CR1_RESTORE;
	
	
  return (0);                                             // Done
}


////unsigned long Verify (unsigned long adr, unsigned long sz, unsigned char *buf) {


////  FLASH->ACR  = 0x00000005;                             // 5 Wait State, no Cache, no Prefetch	
////	
////  while (sz) {
////    if (M32(adr) != *((unsigned long *)buf)) {
////      return(adr);                              // failed
////    }

////    while (FLASH->SR & FLASH_BSY) {
////     // IWDG->KR = 0xAAAA;                        // Reload IWDG
////    }

////    adr += 4;
////    buf += 4;
////    sz  -= 4;

////	}
////  return (adr + sz);                            // Done
////	}
