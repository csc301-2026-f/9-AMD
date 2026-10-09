{{ fullname | escape | underline}}

.. currentmodule:: {{ module }}

.. autoclass:: {{ objname }}
   :members:
   :undoc-members:
   :show-inheritance:
   :special-members: __init__

   {% block methodsummary %}

   {% if methods %}
   .. rubric:: {{ _('Methods') }}

   .. autosummary::
   {% for item in methods %}
      ~{{ name }}.{{ item }}
   {%- endfor %}
   {% endif %}
   {% endblock %}

   {% block attributes %}
   {% if attributes %}
   .. rubric:: {{ _('Attributes') }}

   .. autosummary::
   {% for item in attributes %}
      ~{{ name }}.{{ item }}
   {%- endfor %}
   {% endif %}
   {% endblock %}


{% block datatypes %}

   {% if types %}
   .. rubric:: DataTypes

   .. autosummary::
      :toctree:
   {% for item in types %}
      {{ item }}
   {%- endfor %}

   {% endif %}
   {% endblock %}
