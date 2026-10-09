Make sure pip is updated before installing the schola python package from `Plugins/Schola/Resources/python`.

.. code-block:: bash
    
    python -m pip install --upgrade pip
    pip install ./Plugins/Schola/Resources/python[all]

.. note:: 
        
    Schola installs the cpu version of pytorch by default, to install other versions of pytorch follow the instructions at `Pytorch Get Started <https://pytorch.org/get-started/locally/>`_.

.. note:: 

    To install Pytorch with ROCm on Linux, we recommend following the guide at `Install Pytorch for Radeon GPUs <https://rocm.docs.amd.com/projects/radeon/en/latest/docs/install/native_linux/install-pytorch.html#>`_.