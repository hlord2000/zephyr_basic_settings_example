# Zephyr Basic Settings Demo
This sample demonstrates how to use the Zephyr settings subsystem for persistence across reboots. It also uses external flash as its non-volatile storage backend on supported boards.

## Boards with External Flash Support
- nRF5340DK
- nRF52840DK

In order to use external flash, define a partition as follows:
```c
/ {
    chosen {
        zephyr,settings-partition = &<YOUR_PARTITION_NODELABEL>;
    };
};

&<YOUR_FLASH_NODE> {
    partitions {
        compatible = "fixed-partitions";
        #address-cells = <1>;
        #size-cells = <1>;

        <YOUR_PARTITION_NODELABEL>: partition@0 {
            label = "settings";
            reg = <0x0 0x10000>;
        };
    };
};
```

If a zephyr,settings-partition chosen node is not selected, the settings subsystem will automatically select a fixed partition with the label "storage."

Other options are available for settings backends, including using a file system or internal circular buffer in flash. These options are not enabled in this example.