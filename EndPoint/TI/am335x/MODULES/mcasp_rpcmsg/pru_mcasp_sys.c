#include "mcasp_rpmsg_mod.h"

/* Functions prototypes */
static ssize_t pru_mcasp_show   (
                                struct device * dev,
                                struct device_attribute * attr,
                                char * buf
                                );
/*--------------------------------------------------------------------*/


static DEVICE_ATTR( last_tx_msg_ts1,  S_IRUGO | S_IWUSR, pru_mcasp_show, NULL);
static DEVICE_ATTR( last_rx_msg_ts1,  S_IRUGO | S_IWUSR, pru_mcasp_show, NULL);
static DEVICE_ATTR( last_tx_msg_ts2,  S_IRUGO | S_IWUSR, pru_mcasp_show, NULL);
static DEVICE_ATTR( last_rx_msg_ts2,  S_IRUGO | S_IWUSR, pru_mcasp_show, NULL);
/*--------------------------------------------------------------------*/

static void timespec_period (
                            struct timespec * frst,
                            struct timespec * last,
                            struct timespec * prd //return
                            )
{
    if (last->tv_nsec >= frst->tv_nsec) /* Direct substraction */
    { 
        prd->tv_nsec = last->tv_nsec - frst->tv_nsec;
        prd->tv_sec  = last->tv_sec  - frst->tv_sec;
    }
    else /* Substraction with overflow */
    {
        prd->tv_nsec = 1000000000 + last->tv_nsec - frst->tv_nsec;
        prd->tv_sec  =             (last->tv_sec  - frst->tv_sec) - 1;
    }
}

static ssize_t pru_mcasp_show   (
                                struct device * dev,
                                struct device_attribute * attr,
                                char * buf
                                ) {
    struct statistics * stat_ptr = NULL;
    struct timespec prd = { 0 };
    
    struct pru_mcasp_device * pru_mcasp_dev_ptr = dev_get_drvdata(dev);
    if (!pru_mcasp_dev_ptr) { __PRINTERR("Driver data not provided.\n"); }    
    stat_ptr = &pru_mcasp_dev_ptr->stat;   
 
    if (attr ==                      (struct device_attribute *)&dev_attr_last_tx_msg_ts1.attr) {
                                               timespec_period(&stat_ptr->frst_tx_msg_ts1,
                                                               &stat_ptr->last_tx_msg_ts1, &prd);
        sprintf (buf, "%lu.%lu:%llu",  prd.tv_sec, prd.tv_nsec, stat_ptr->cntr_tx_msg_ts1); 
    }
    if (attr ==                      (struct device_attribute *)&dev_attr_last_tx_msg_ts2.attr) {
                                               timespec_period(&stat_ptr->frst_tx_msg_ts2,
                                                               &stat_ptr->last_tx_msg_ts2, &prd);
        sprintf (buf, "%lu.%lu:%llu",  prd.tv_sec, prd.tv_nsec, stat_ptr->cntr_tx_msg_ts2); 
    }
    if (attr ==                      (struct device_attribute *)&dev_attr_last_rx_msg_ts1.attr) {
                                               timespec_period(&stat_ptr->frst_rx_msg_ts1,
                                                               &stat_ptr->last_rx_msg_ts1, &prd);
        sprintf (buf, "%lu.%lu:%llu",  prd.tv_sec, prd.tv_nsec, stat_ptr->cntr_rx_msg_ts1); 
    }
    if (attr ==                      (struct device_attribute *)&dev_attr_last_rx_msg_ts2.attr) {
                                               timespec_period(&stat_ptr->frst_rx_msg_ts2,
                                                               &stat_ptr->last_rx_msg_ts2, &prd);
        sprintf (buf, "%lu.%lu:%llu",  prd.tv_sec, prd.tv_nsec, stat_ptr->cntr_rx_msg_ts2); 
    }
    return strlen(buf);
}

/* Declare attributes table (listing in fact sysfs files for driver) */
static struct attribute * pru_mcasp_attrs[] = 
{
    &dev_attr_last_tx_msg_ts1.attr,
    &dev_attr_last_rx_msg_ts1.attr,
    &dev_attr_last_tx_msg_ts2.attr,
    &dev_attr_last_rx_msg_ts2.attr,
    NULL
};
/* Declare attribute group (specifying in fact sysfs directory for your driver) */
struct attribute_group pru_mcasp_group = 
{
    .name  = "pru_mcasp",
    .attrs = pru_mcasp_attrs,
};

