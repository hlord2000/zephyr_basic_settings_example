#include <zephyr/kernel.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>

LOG_MODULE_REGISTER(main);


/* STEP 1: Let's create a structure that we want to save */
struct my_settings_t {
    uint32_t reboots;
    uint32_t uptime;
};

struct my_settings_t my_value;

/* STEP 2: Let's also use a plain atomic data type */
uint32_t my_atomic_value = 0xCAFFE19E;

/* STEP 4: Create our handler function, comparing the name of the setting */
int settings_handle_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
    const char *next;
    int err;

    if (settings_name_steq(name, "my_value", &next) && !next) {
        if (len != sizeof(my_value)) {
            return -EINVAL;
        }

        err = read_cb(cb_arg, &my_value, sizeof(my_value));
        if (err <= 0) {
            return err;
        }

        return 0;
    }

    else if (settings_name_steq(name, "my_atomic_value", &next) && !next) {
        if (len != sizeof(my_atomic_value)) {
            return -EINVAL;
        }

        err = read_cb(cb_arg, &my_atomic_value, sizeof(my_atomic_value));
        if (err <= 0) {
            return err;
        }

        return 0;
    }

    return -ENOENT;
}

/* STEP 3: Let us set our settings handler functions. For this example, we will only define how to set 
 *   our different types of data.
 */

struct settings_handler my_conf = {
    .name = "my_settings",
    .h_set = settings_handle_set,
};

int main(void)
{
    int err;
    /* STEP 5: Initialize the settings subsystem */
    err = settings_subsys_init();
    if (err) {
        LOG_ERR("settings_subsys_init failed (err %d)", err);
    }

    /* STEP 6: Register our settings configuration. Note: there are a number of
     * different ways to accomplish this. You can define a handler statically, or
     *  not use a handler at all
     */
    err = settings_register(&my_conf);
    if (err) {
        LOG_ERR("settings_register failed (err %d)", err);
    }

    /* STEP 7: We load all settings at boot with "settings_load" */
    err = settings_load();
    if (err) {
        LOG_ERR("settings_load failed (err %d)", err);
    }

    k_msleep(5000);

    LOG_INF("Reboots: %u, uptime: %u", my_value.reboots, my_value.uptime);
    LOG_INF("Atomic value: 0x%08X", my_atomic_value);
    
    my_value.reboots++;
    my_value.uptime = k_uptime_get_32();

    if (my_value.reboots % 2) {
        my_atomic_value = 0xDEADBEEF;
    }
    else {
        my_atomic_value = 0xCAFFE19E;
    }

    /* STEP 8: Save the settings */
    err = settings_save_one("my_settings/my_value", &my_value, sizeof(my_value));
    if (err) {
        LOG_ERR("Cannot save settings (err %d)", err);
    }

    err = settings_save_one("my_settings/my_atomic_value", &my_atomic_value, sizeof(my_atomic_value));
    if (err) {
        LOG_ERR("Cannot save settings (err %d)", err);
    }

    k_msleep(1000);

    /* STEP 9: Reboot the system and see that the results persist */
    sys_reboot(SYS_REBOOT_COLD);
}
