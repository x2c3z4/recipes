SCSI C/S模型
============

1. 首先initiator发送命令，target发送数据和状态。如果失败，initiator发送sence command， target返回sense data表示错误原因。

![C/S 模型](http://www.ibm.com/developerworks/library/l-scsi-api/figure1.gif)

2. 每个SCSI命令都由一个CDB来描述。

command           | description
----------------------------------------------------------------------------
Inquiry           | Requests general information of the target device
Test/Unit/Ready   | Checks whether the target device is ready for the transfer operation
READ              | Transfers data from the SCSI target device
WRITE             | Transfers data to the SCSI target device
Request           | Sense Requests the sense data of the last command
Read              | Capacity Requests the storage capacity information


3.

![Layer of SCSI](http://www.ibm.com/developerworks/library/l-scsi-api/figure2.gif)

4. 使用SCSI generic命令

  - 打开SCSI generic device
  - 准备SCSI命令
  - 设置相关的buffer
  - 调用ioctl执行SCSI命令
  - 关闭设备

5. ioctl(fd, SG_IO, p_io_hdr)
  最重要的数据结构是 struct sg_io_hdr

6. linux/include/scsi/sg.h#43

```{.c}
43typedef struct sg_io_hdr
44{
45    int interface_id;           /* [i] 'S' for SCSI generic (required) */
46    int dxfer_direction;        /* [i] data transfer direction  */
47    unsigned char cmd_len;      /* [i] SCSI command length */
48    unsigned char mx_sb_len;    /* [i] max length to write to sbp */
49    unsigned short iovec_count; /* [i] 0 implies no scatter gather */
50    unsigned int dxfer_len;     /* [i] byte count of data transfer */
51    void __user *dxferp;  /* [i], [*io] points to data transfer memory
52                or scatter gather list */
53    unsigned char __user *cmdp; /* [i], [*i] points to command to perform */
54    void __user *sbp;   /* [i], [*o] points to sense_buffer memory */
55    unsigned int timeout;       /* [i] MAX_UINT->no timeout (unit: millisec) */
56    unsigned int flags;         /* [i] 0 -> default, see SG_FLAG... */
57    int pack_id;                /* [i->o] unused internally (normally) */
58    void __user * usr_ptr;      /* [i->o] unused internally */
59    unsigned char status;       /* [o] scsi status */
60    unsigned char masked_status;/* [o] shifted, masked scsi status */
61    unsigned char msg_status;   /* [o] messaging level data (optional) */
62    unsigned char sb_len_wr;    /* [o] byte count actually written to sbp */
63    unsigned short host_status; /* [o] errors from host adapter */
64    unsigned short driver_status;/* [o] errors from software driver */
65    int resid;                  /* [o] dxfer_len - actual_transferred */
66    unsigned int duration;      /* [o] time taken by cmd (unit: millisec) */
67    unsigned int info;          /* [o] auxiliary information */
68} sg_io_hdr_t;  /* 64 bytes long (on i386) */
69
```

描述：
- dxfer_direction:

  73#define SG_DXFER_NONE (-1)      /* e.g. a SCSI Test Unit Ready command */
  74#define SG_DXFER_TO_DEV (-2)    /* e.g. a SCSI WRITE command */
  75#define SG_DXFER_FROM_DEV (-3)  /* e.g. a SCSI READ command */
  76#define SG_DXFER_TO_FROM_DEV (-4) /* treated like SG_DXFER_FROM_DEV with the
  77           additional property than during indirect
  78           IO the user buffer is copied into the
  79           kernel buffers before the transfer */
  80#define SG_DXFER_UNKNOWN (-5)   /* Unknown data direction */

- cmd_len = len(cmdp)，命令长度
- mx_sb_len, sb_len_wr = len(sbp)， sense长度
- dxfer_len 传输的字节数 = len(dxferp)


7. 执行inquiry命令



References
==========


1. http://www.ibm.com/developerworks/library/l-scsi-api/
