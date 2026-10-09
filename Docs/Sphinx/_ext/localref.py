# Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

from docutils import nodes  
from docutils.parsers.rst import roles  
from sphinx.util.nodes import split_explicit_title  
  
def localref_role(name, rawtext, text, lineno, inliner, options={}, content=[]):  
    # Split the text into title and target if explicit title is provided  
    has_explicit_title, title, target = split_explicit_title(text)  
  
    # If no explicit title is provided, use the target as the title  
    if not has_explicit_title:  
        title = target  
  
    full_target = nodes.make_id(target) 
    refnode = nodes.reference(rawtext, title, refuri="#"+full_target, internal=True)  
  
    return [refnode], []  
  
def setup(app):  
    # Register the localref role  
    roles.register_local_role('localref', localref_role)  
  
    return {  
        'version': '0.1',  
        'parallel_read_safe': True,  
        'parallel_write_safe': True,  
    }