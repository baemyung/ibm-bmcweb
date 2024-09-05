# IBM schemas that should be installed
schemas = {
    'IBMAssembly',
    'IBMFabricAdapter',
    'IBMLogEntryAttachment',
    'IBMLogEntry',
    'IBMManagerAccount',
    'IBMMessage',
    'IBMPCIeDevice',
    'IBMPCIeSlots',
    'IBMServiceRoot',
}

foreach schema : schemas
    install_data(
        'csdl/@0@_v1.xml'.format(schema),
        install_dir: '/var/www/redfish/v1/schema',
        follow_symlinks: true,
    )

    install_data(
        'json-schema/@0@.json'.format(schema),
        install_dir: '/var/www/redfish/v1/JsonSchema/@0@'.format(schema),
        follow_symlinks: true,
    )
endforeach
