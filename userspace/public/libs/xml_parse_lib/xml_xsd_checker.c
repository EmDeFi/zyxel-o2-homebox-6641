/*-----------------------------------------------------------------------------------------
  XML Schema Definition (XSD) Checker -

   These routines check parsed XML for consistency with an XSD schema.
   Checks that:
    - "schema" is root tag of the schema definition.
    - tag-names are in the admissible set and positions (hierarchy/sequence).
    - attribute types are consistent.  (String is a no-op, but float and int are checked.)
    - simple elements are not complex.

 Inputs:    Supply two xml-token-trees:
	xml_schema_tree
	xml_data_tree

 Outputs:
	Returns 0 on success, otherwise returns number of errors.
	Retports errors to screen as encountered.

 Compile:
  cc -g xml_xsd_checker.c -lm -o xml_xsd_checker.exe
 ------------------------------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "xml_parse_lib.h"
#define xsd_maxstr 5000


int xsd_check( Xml_object *xml_schema_tree, Xml_object *xml_data_tree )
{
 int errcnt=0, level=0;;
 char tag1[xsd_maxstr], contents1[xsd_maxstr], tag2[xsd_maxstr], contents2[xsd_maxstr],
	attrib1[xsd_maxstr], value1[xsd_maxstr];
 struct xml_private_tree *xsd_tree, *xml_tree;

 xml_tree_start_traverse( &xsd_tree, xml_schema_tree, tag1, contents1, xsd_maxstr );
 while ((xsd_tree) && (tag1[0]=='?')) {xml_tree_descend_to_child( &xsd_tree, tag1, contents1, xsd_maxstr );}

//{xml_tree_get_next_tag( xsd_tree, tag1, contents1, xsd_maxstr );  printf("tag1='%s'\n",tag1);}
 if (strcmp(tag1,"xs:schema")!=0)
  { errcnt++; printf("XSD Error: Schema does not start with 'xs:schema' tag, but with '%s'.\n",tag1); }

 /* Now traverse the data-tree, checking against the schema. */
#if(0)
 do
  {
   if (xml_tree_descend_to_child( &xsd_tree, tag1, contents1, xsd_maxstr ))             /* Get children, if any. */
    level++;
   else         /* Otherwise sequence to next tag at present level. */
    {                                                                           /* Else get next tag, if any. */
     while ((xsd_tree) && (! xml_tree_get_next_tag( xsd_tree, tag1, contents1, xsd_maxstr )))
      {         /* Go up, if no more nodes at this level. */
       level--;
       xml_tree_ascend( &xsd_tree );                                            /* Otherwise, go up. */
      }
    }
  }
 while (strcmp(attrib1,"name")!=0);
#endif

 xml_tree_descend_to_child( &xsd_tree, tag1, contents1, xsd_maxstr );
 xml_tree_get_next_attribute( xsd_tree, attrib1, value1, xsd_maxstr );
 if (strcmp(attrib1,"name")!=0)
  { errcnt++; printf("XSD Error: Schema attribute not 'name', but '%s'.\n",attrib1); }

 xml_tree_start_traverse( &xml_tree, xml_data_tree, tag2, contents2, xsd_maxstr );
 while ((xml_tree) && (tag2[0]=='?')) xml_tree_descend_to_child( &xml_tree, tag2, contents2, xsd_maxstr );
 if (strcmp(value1,tag2)!=0)
  { errcnt++; printf("XSD Error: Xml tag '%s' does not match Schema '%s'.\n", tag2, value1); }


  /* Descend into this tag's children, if posible. */
   if (xml_tree_descend_to_child( &xml_tree, tag2, contents2, xsd_maxstr ))		/* Get children, if any. */
    level++;
   else		/* Otherwise sequence to next tag at present level. */
    {										/* Else get next tag, if any. */
     while ((xml_tree) && (! xml_tree_get_next_tag( xml_tree, tag2, contents2, xsd_maxstr )))
      {		/* Go up, if no more nodes at this level. */
       level--;
       xml_tree_ascend( &xml_tree );						/* Otherwise, go up. */
      }
    }

 
 return errcnt;
}




#define maxstr 5000
void indent( int level )
{
 int j;
 for (j=0; j<level; j++) printf("  ");		/* Indent appropriate to level. */
}

void traverse( Xml_object *rootobj )
{
 struct xml_private_tree *xml_tree;
 char tag[maxstr], contents[maxstr], attrib[maxstr], value[maxstr];
 int  level=0; 

 xml_tree_start_traverse( &xml_tree, rootobj, tag, contents, maxstr );		/* Initiate xml-traversal. */
 while (xml_tree)
  {
   indent(level);			/* Indent appropriate to level. */	
   printf("Tag='%s'\n", tag);		/* Show the tag-name. */
   while (xml_tree_get_next_attribute( xml_tree, attrib, value, maxstr ))	/* Get next attribute. */
    {					/* Show the attribtes. */
     indent(level);			/* Indent appropriate to level. */
     printf(" Attribute: %s = '%s'\n", attrib, value);
    }
   if (strlen(contents)>0)
    {					/* Show the contents. */
     indent(level);
     printf(" Contents: '%s'\n", contents);
    }

   /* Descend into this tag's children, if posible. */
   if (xml_tree_descend_to_child( &xml_tree, tag, contents, maxstr ))		/* Get children, if any. */
    level++;
   else		/* Otherwise sequence to next tag at present level. */
    {										/* Else get next tag, if any. */
     while ((xml_tree) && (! xml_tree_get_next_tag( xml_tree, tag, contents, maxstr )))
      {		/* Go up, if no more nodes at this level. */
       level--;
       xml_tree_ascend( &xml_tree );						/* Otherwise, go up. */
      }
    }
  }
}
