/*
 * microElk
 * Last changes
 * 2018.03.22
 */
#include "GPIO_spplmntl.h"

int32_t Prepare_GPIO(
        int32_t gpio_num,
        int32_t direction
        ) {
    int fd, retval;
    char * buf = (char *)malloc(BUFFER_MAX_SIZE);
    TEST_MALLOC(buf);
    bzero(buf, BUFFER_MAX_SIZE);

    fd = open("/sys/class/gpio/unexport", O_WRONLY | S_IWUSR);
    if (fd < 0) { FPRINTF_EXT("GPIO #%i.\r\n", gpio_num); goto failure; }

    sprintf(buf, "%d", gpio_num);
    retval = write(fd, buf, strlen(buf));
    close(fd);
    if (retval < 0) { FPRINTF_EXT("GPIO #%i.\r\n", gpio_num); }

    /* Reserve (export) the GPIO */
    fd = open("/sys/class/gpio/export", O_WRONLY | S_IWUSR);
    if (fd < 0) { FPRINTF_EXT("GPIO #%i.\r\n", gpio_num);  goto failure; }

    sprintf(buf, "%d", gpio_num);
    retval = write(fd, buf, strlen(buf));
    close(fd);
    if (retval < 0) {
        FPRINTF_EXT("GPIO #%i.\r\n", gpio_num);
        goto failure;
    }

    /* Set the direction */
    sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio_num);
    fd = open(buf, O_WRONLY | S_IWUSR);
    if (fd < 0) {
        FPRINTF_EXT("GPIO #%i.\r\n", gpio_num);
        goto failure;
    }

    if (!direction) {
        retval = write(fd, "out", strlen("out")); /* Set 'out' direction */
        if (retval != strlen("out")) {
            FPRINTF_EXT("GPIO #%i.\r\n", gpio_num);
            close(fd);
            goto failure;
        }
    } else {
        retval = write(fd, "in", strlen("in")); /* Set 'in' direction */
        if (retval != strlen("in")) {
            FPRINTF_EXT("GPIO #%i.\r\n", gpio_num);
            close(fd);
            goto failure;
        }
    }
    close(fd);
    free(buf);
    return EXIT_SUCCESS;

    failure:
    PRINT_WARN("Can't prepare GPIO %i.\r\n", gpio_num);
    free(buf);
    return EXIT_FAILURE;
}
/*----------------------------------------------------------------------------*/

int Free_GPIO(uint32_t gpio_num) {
    int fd, retval;
    char * buf = (char *)malloc(BUFFER_MAX_SIZE);
    TEST_MALLOC(buf);
    /* Reserve (export) the GPIO */
    fd = open("/sys/class/gpio/unexport", O_WRONLY | S_IWUSR);
    if (fd < 0) {
        FPRINTF_EXT("GPIO #%i.\r\n", gpio_num);
        goto failure;
    }
    sprintf(buf, "%d", gpio_num);
    retval = write(fd, buf, strlen(buf));
    close(fd);
    if (retval < 0) {
        FPRINTF_EXT("GPIO #%i.\r\n", gpio_num);
        goto failure;
    }

    free(buf);
    return EXIT_SUCCESS;

    failure:
    free(buf);
    PRINT_WARN("Can't unexport GPIO %i.\r\n", gpio_num);
    return EXIT_FAILURE;

}
/*----------------------------------------------------------------------------*/

int Get_GPIO_val(uint32_t gpio_num) {
    int fd, retval;
    char * buf = (char *)malloc(BUFFER_MAX_SIZE);
    TEST_MALLOC(buf);
    bzero(buf, BUFFER_MAX_SIZE);

    if (Prepare_GPIO(gpio_num, GPIO_DIRECTION_IN) != EXIT_SUCCESS)
        goto failure;

    sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio_num);
    fd = open(buf, O_RDONLY | S_IWUSR);
    if (fd < 0) {
        FPRINTF_EXT("GPIO #%i.\r\n", gpio_num);
        goto failure;
    }

    bzero(buf, BUFFER_MAX_SIZE);
    retval = read(fd, buf, 1);
    close(fd);
    if (retval < 0) {
        FPRINTF_EXT("GPIO #%i.\r\n", gpio_num);
        goto failure;
    }

    retval = atoi(buf);
    if (Free_GPIO(gpio_num) != EXIT_SUCCESS) {
        goto failure;
    }
    free(buf);
    return (retval);

    failure:
    free(buf);
    PRINT_WARN("Can't get GPIO %i value.\r\n", gpio_num);
    Free_GPIO(gpio_num);
    return -1;
}
/*----------------------------------------------------------------------------*/

int Set_GPIO_val(uint32_t gpio_num, uint32_t value) {
    int fd, retval = 0;
    char * buf = (char *)malloc(BUFFER_MAX_SIZE);
    TEST_MALLOC(buf);
    bzero(buf, BUFFER_MAX_SIZE);

    if (Prepare_GPIO(gpio_num, GPIO_DIRECTION_OUT) != EXIT_SUCCESS)
        goto failure;

    sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio_num);
    fd = open(buf, O_WRONLY | S_IWUSR);
    if (fd < 0) {
        FPRINTF_EXT("GPIO #%i.\r\n", gpio_num);
        goto failure;
    }

    bzero(buf, BUFFER_MAX_SIZE);
    sprintf(buf, "%d", value);
    retval = write(fd, buf, strlen(buf));
    close(fd);
    if (retval < 0) {
        FPRINTF_EXT("GPIO #%i.\r\n", gpio_num);
        goto failure;
    }
    free(buf);
    return (retval);

    failure:
    free(buf);
    PRINT_WARN("Can't get GPIO %i value.\r\n", gpio_num);
    Free_GPIO(gpio_num);
    return -1;
}
/*----------------------------------------------------------------------------*/


