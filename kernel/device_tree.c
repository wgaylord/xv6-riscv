#include "device_tree.h"

static inline uint32 bswap(uint32 x)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  uint32 y = (x & 0x00FF00FF) <<  8 | (x & 0xFF00FF00) >>  8;
  uint32 z = (y & 0x0000FFFF) << 16 | (y & 0xFFFF0000) >> 16;
  return z;
#else
  /* No need to swap on big endian */
  return x;
#endif
}

void *fdt_pointer;

void
device_tree_init(uint32* devicetree)
{
    printf("Getting Device Tree\n");
    struct fdt_header *dtb_header = (struct fdt_header *) devicetree;
    int i = 0;
    int total_size = bswap(dtb_header->totalsize);
    if(bswap(dtb_header->magic)==0xd00dfeed){
        fdt_pointer = kalloc();
        for(i=4096;i<total_size;i+=4096){
            kalloc();
        }
        printf("Allocated  %d bytes for Device Tree that is %d bytes in size.\n",i,total_size);
        printf("Copying Device Tree\n");
        for(i=0;i<total_size;i++){
            ((uint8*)fdt_pointer)[i] = ((uint8*)dtb_header)[i];
        }
        printf("Releasing original Device Tree memory\n");
    }else{
        panic("Device Tree is not valid!");
    }
    
    struct fdt_reserve_entry  *reserve_mem_entry = fdt_pointer + bswap(((struct fdt_header *)fdt_pointer)->off_mem_rsvmap);
    printf("Getting Reserved Memory\n");
    i = 0;
    while(!(reserve_mem_entry[i].address == 0 && reserve_mem_entry[i].size == 0)){
        printf("Entry %d\n",i);
        printf("    Address: %p\n",reserve_mem_entry[i].address);
        printf("    Size: %d\n",reserve_mem_entry[i].size);
        i++;
    }
    if(i==0){
        printf("No Reserved Memory found in FDT.\n");
    }
    
    i = 0;
    void*  structure_block = fdt_pointer + bswap(((struct fdt_header *)fdt_pointer)->off_dt_struct);
    printf("Struture Memory %p\n",structure_block);
    while(1){
        if(bswap(*(uint32*)(structure_block + i)) == 1){
            i += 4;
            printf("FDT_BEGIN_NODE - %s\n",structure_block+i);
            i = roundup_32(strlen(structure_block+i)+i+1);
        }
        if(bswap(*(uint32*)(structure_block + i)) == 2){
            printf("FDT_END_NODE\n");
            i+=4;
        }
        if(bswap(*(uint32*)(structure_block + i)) == 3){
            printf("FDT_PROP\n");
            i+=4;
            int len = bswap(*(uint32*)(structure_block + i));
            i +=4;
            int string_offset = bswap(*(uint32*)(structure_block + i));
            printf("String Offset %d\n",string_offset);
            printf("Name: %s\n",(char *)(fdt_pointer + bswap(((struct fdt_header *)fdt_pointer)->off_dt_strings) + string_offset));
            i +=4;
            printf("Length %d\n",len);
            if(len == 4){
                printf("Data: %d\n",bswap(*(uint32*)(structure_block + i)));
            }
            if(len == 16){
                printf("Data: %p %p %p %p\n",bswap(*(uint32*)(structure_block + i)),bswap(*(uint32*)(structure_block + i+4)),bswap(*(uint32*)(structure_block + i+8)),bswap(*(uint32*)(structure_block + i+12)));
            }
            i = roundup_32(len+i);
        }
        if(bswap(*(uint32*)(structure_block + i)) == 4){
            printf("FDT_NOP\n");
            i+=4;
        }
        if(bswap(*(uint32*)(structure_block + i)) == 9){
            printf("FDT_END\n");
            break;
        }
        if(i > bswap(((struct fdt_header *)fdt_pointer)->size_dt_struct)){
            break;
        }
    }
}
