#! /usr/sbin/dtrace -s
#pragma D option quiet

BEGIN
{
  self->boot_args = ((struct boot_args*)(`PE_state).bootArgs);
  self->deviceTreeHead = ((struct boot_args*)(`PE_state).deviceTreeHead);
  self->video = ((PE_Video)(`PE_state).video);
  printf("EFI: %d-bit\n", self->boot_args->efiMode);
  printf("Video: Base Addr: %p\n", self->video.v_baseAddr);
  printf("Video resolution: %dx%dx%d\n", self->video.v_width,self->video.v_height,self->video.v_depth);
  printf("Kernel Command line: %s\n", self->boot_args->CommandLine);
  printf("Kernel begins at physical address 0x%x and spans %d bytes\n", self->boot_args->kaddr,self->boot_args->ksize);
  printf("Device Tree begins at physical address 0x%x and spans %d bytes\n", self->boot_args->deviceTreeP, self->boot_args->deviceTreeLength);
  printf("Memory Map of %d bytes resides in physical address 0x%x", self->boot_args->MemoryMapSize, self->boot_args->MemoryMap);
}
