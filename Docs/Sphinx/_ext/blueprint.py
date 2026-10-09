# Copyright (c) 2024 Advanced Micro Devices, Inc. All Rights Reserved.

import logging
from pathlib import Path
from sphinx.application import Sphinx

logger = logging.getLogger(__name__)
import sphinx.builders
import sphinx.parsers
import sphinx.transforms
from sphinx.util.docutils import SphinxDirective, SphinxRole
from sphinx.util.typing import ExtensionMetadata
import sphinx
from docutils import nodes
from sphinx.writers.html5 import HTML5Translator
from docutils.parsers.rst import directives

class blueprint(nodes.General, nodes.Element):
    pass

#Don't need to define a role

class BlueprintDirective(SphinxDirective):

    has_content=True
    required_arguments = 0
    optional_arguments = 0
    option_spec =  {
        "imagefallback": directives.path,
        "height": directives.positive_int,
        "zoom": int,
        "heading": str,
    }
    
    def run(self) -> list[nodes.Node]: 
        if ("html" in self.env.app.builder.name):
            height = self.options["height"] if "height" in self.options else 500
            zoom = self.options["zoom"] if "zoom" in self.options else 0
            heading = self.options["heading"] if "heading" in self.options else ""
            imagefallback = self.options["imagefallback"] if "imagefallback" in self.options else None
            return [blueprint(code="\n".join(self.content),height=height,imagefallback=imagefallback,zoom=zoom,heading=heading)]
        else:
            if "imagefallback" in self.options:
                return [nodes.image(uri=self.options["imagefallback"])]
            else:
                return []

class BlueprintFileDirective(SphinxDirective):

    has_content=False
    required_arguments = 1
    optional_arguments = 0
    option_spec =  {
        "imagefallback": directives.path,
        "height": directives.positive_int,
        "zoom": int,
        "heading": str,
    }
    
    def run(self) -> list[nodes.Node]: 
        bp_dir = self.env.app.confdir / Path(self.env.app.config.blueprint_dir)
        file_path = bp_dir / Path(self.arguments[0])
        if ("html" in self.env.app.builder.name) and file_path.exists():
            height = self.options["height"] if "height" in self.options else 500
            zoom = self.options["zoom"] if "zoom" in self.options else 0
            heading = self.options["heading"] if "heading" in self.options else ""
            imagefallback = self.options["imagefallback"] if "imagefallback" in self.options else None
            code = ""
            with open(file_path) as file:
                code = file.readlines()
                #remove any rst comments
                code = "\n".join(filter(lambda x: not ".. " in x, code))
            #empty bp on fail to load
            return [blueprint(code=code,height=height,imagefallback=imagefallback,zoom=zoom,heading=heading)]
        else:
            if "imagefallback" in self.options:
                return [nodes.image(uri=self.options["imagefallback"])]
            else:
                return []


def visit_blueprint_node(self : HTML5Translator, node: blueprint):
    html_node = f"""<style>
        #ueb-id-{visit_blueprint_node.id} {{
            --ueb-height: {node.attributes["height"]}px;
        }}
        template {{
            display : block
        }}
    </style>
    <ueb-blueprint data-number-id="{visit_blueprint_node.id}" data-heading="{node.attributes["heading"].replace(">"," ❯ ")}" data-zoom="{node.attributes["zoom"]}" id="ueb-id-{visit_blueprint_node.id}">
        <template id="template-id-{visit_blueprint_node.id}">
            {node.attributes["code"]}
    """
    if node.attributes["imagefallback"] != None:
        logger.debug("%s", node.attributes["imagefallback"])
        html_node = f"""<noscript>  
            <img src="{node.attributes["imagefallback"]}" alt="{node.attributes["heading"]} Fallback Image">  
        </noscript>""" + html_node
    self.body.append(html_node)
    visit_blueprint_node.id += 1
visit_blueprint_node.id = 0 #it could break if sphinx imports the plugin multiple times etc.

def depart_blueprint_node(self : HTML5Translator, node: blueprint):
    self.body.append("""
        </template>
    </ueb-blueprint>
    """)



class BlueprintNodeSearcher(nodes.SparseNodeVisitor):

    def visit_blueprint(self, node):
        self.found_bp_node = True
        raise nodes.NodeFound(node)

def update_context(app: Sphinx, pagename, templatename, context, doctree):
    if doctree is None:
        return
    try:
        searcher = BlueprintNodeSearcher(doctree)
        doctree.walk(searcher)
    except nodes.NodeFound:
        app.add_js_file(filename=None,type="module",body='import { Blueprint } from "/_static/ueblueprint.js"')
        app.add_css_file(filename="css/ueb-style.css")
    
def setup(app: Sphinx) -> ExtensionMetadata:
    
    app.add_node(blueprint,html=(visit_blueprint_node,depart_blueprint_node))
    app.add_directive('blueprint', BlueprintDirective)
    app.add_directive('blueprint-file', BlueprintFileDirective)
    app.add_config_value('blueprint_dir', Path('./blueprints'), 'env', [Path,str])

    # link our extension static files to the output page
    static_dir = Path(__file__).parent / "static"
    app.connect(
        "builder-inited",
        (lambda app: app.config.html_static_path.append(static_dir.as_posix())),
    )

    app.connect("html-page-context", update_context)
    
    return {
        'version': '0.1',
        'parallel_read_safe': True,
        'parallel_write_safe': True,
    }
