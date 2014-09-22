/* -------------------------------------------------------------------------
Test_xml_token_traverser.c - A simple example of calling re-usable xml-parsing
 library for reading an arbitrary xml-file, and traversing through the
 tokens.  Reads xml-file(s) on command-line into token-tree, then traverses
 the tokens.

Compile:
 cc -g -O test_xml_token_traverser.c -lm -o test_xml_token_traverser.exe

Run:
 ./test_xml_token_traverser.exe  yourxmlfile.xml
-------------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/stat.h>
#include <unistd.h>

#include "xml_parse_lib.h"
#include "cms_log.h"				 /*irene partial test*/
#include "cms_image.h"
#define maxstr XML_MAX_STRLEN
#include "cms_image.h"


int filter_password( Xml_object *rootobj, Xml_object *target, FILE *output_fs );

int merge_password( Xml_object *rootobj, Xml_object *target,Xml_object *filter, FILE *output_fs);

#if 1//def ZLD_GLOB_TTNET_FEATURE /* Open compiler flag "ZLD_GLOB_TTNET_FEATURE" as common at this moment, temporarily. */
                                  /* Further work will discuss the protect datamodel of TTNET into common feature. */
static int needToRecover(Xml_object *target, Xml_object *filter, Xml_object **keeper);
static int is_ix_style(Xml_object *node);
static int is_iN_style(Xml_object *node);
static void delete_node(Xml_object *node);
static void value_validate(Xml_object *target, Xml_object *rtobj);
static void fullize_value(Xml_object *rtobj, Xml_object *follower);
static void do_traverse(Xml_object *target, Xml_object *rtobj,Xml_object *follower);
int traverse_tree( Xml_object *rootobj, Xml_object *target,Xml_object *follower, FILE* output_fs);
int config_proc(int mode,Xml_object *rootobj,Xml_object *filter_obj,Xml_object *pro_obj, FILE * output_fs);

#endif


int filter_password( Xml_object *rootobj, Xml_object *target, FILE *output_fs )
{
	struct xml_private_tree * xml_tree, *t_xml_tree;
	char tag[ maxstr ], contents[ maxstr ];
	char t_tag[ maxstr ], t_contents[ maxstr ];
	int level=0, t_level= 0;

	/* Initiate xml-traversal. */
	xml_tree_start_traverse( &xml_tree, rootobj, tag, contents, maxstr );
	/* Initiate xml-traversal. */
	xml_tree_start_traverse( &t_xml_tree, target, t_tag, t_contents, maxstr );

	while ( xml_tree )
	{
		/* Initiate xml-traversal. */
		xml_tree_start_traverse( &t_xml_tree, target, t_tag, t_contents, maxstr );
		t_level=0;
		while ( t_xml_tree )
		{
			if ( !strcmp( t_tag, tag )&& (level==t_level) )
			{
				/* Clear the contents. */
				if (strlen( contents ) > 0 )
				{
					memset( xml_tree->traverse_node->contents, 0,
							sizeof(xml_tree->traverse_node->contents));
				}
				break;
			}

			/* Get children, if any. */
			if ( xml_tree_descend_to_child( &t_xml_tree, t_tag, t_contents, maxstr ) )
				t_level++;
			else /* Otherwise sequence to next tag at present level. */
			{ /* Else get next tag, if any. */
				while ( ( t_xml_tree ) &&
						( ! xml_tree_get_next_tag( t_xml_tree, t_tag, t_contents, maxstr ) ) )
				{ /* Go up, if no more nodes at this level. */
					t_level--;
					xml_tree_ascend( &t_xml_tree ); /* Otherwise, go up. */
				}
			}
		}

		/* Get children, if any. */
		if ( xml_tree_descend_to_child( &xml_tree, tag, contents, maxstr ) )
			level++;
		else /* Otherwise sequence to next tag at present level. */
		{ /* Else get next tag, if any. */
			while ( ( xml_tree ) &&
					( ! xml_tree_get_next_tag( xml_tree, tag, contents, maxstr ) ) )
			{ /* Go up, if no more nodes at this level. */
				level--;
				xml_tree_ascend( &xml_tree ); /* Otherwise, go up. */
			}
		}
	}

	Xml_Write_File( output_fs, rootobj );

    return 0;
}

int find_tag(struct xml_private_tree **xml_tree, char *tag, char *contents, int *level, char *f_tag, int f_level)
{
	int found = 0;

	while (*xml_tree)
	{
		if (f_level == *level)
		{
			if (!strcmp( f_tag, tag ))
			{
				found = 1;
				break;
			}
			if (!xml_tree_get_next_tag(*xml_tree, tag, contents, maxstr))
			{
				break;
			}
		}
		else if (f_level > (*level))
		{
			++(*level);
			xml_tree_descend_to_child(xml_tree, tag, contents, maxstr);
		}
		else
		{
			--(*level);
			xml_tree_ascend(xml_tree);
		}
	}

	return found;
}

int merge_password( Xml_object *rootobj, Xml_object *target,Xml_object *filter, FILE* output_fs )
{
	struct xml_private_tree * xml_tree, *t_xml_tree, *f_xml_tree;
	char tag[ maxstr ], contents[ maxstr ];
	char t_tag[ maxstr ], t_contents[ maxstr ];
	char f_tag[ maxstr ], f_contents[ maxstr ];
	int level = 0, t_level = 0, f_level = 0;
	int found, t_found;

	/* Initiate xml-traversal. */
	xml_tree_start_traverse( &xml_tree, rootobj, tag, contents, maxstr );
	/* Initiate xml-traversal. */
	xml_tree_start_traverse( &t_xml_tree, target, t_tag, t_contents, maxstr );
	/* Initiate xml-traversal. */
	xml_tree_start_traverse( &f_xml_tree, filter, f_tag, f_contents, maxstr );

	while (f_xml_tree)
	{
		found = find_tag(&xml_tree, tag, contents, &level, f_tag, f_level);
		t_found = find_tag(&t_xml_tree, t_tag, t_contents, &t_level, f_tag, f_level);
		if ( (found == 0) || (t_found == 0) )
		{
			while ( (f_xml_tree) &&
					(!xml_tree_get_next_tag(f_xml_tree, f_tag, f_contents, maxstr)) )
			{
				f_level--;
				xml_tree_ascend(&f_xml_tree);
			}
			continue;
		}
		if ( (strlen(t_contents) == 0) && (strlen(contents) > 0) )
		{
			free(t_xml_tree->traverse_node->contents);
			t_xml_tree->traverse_node->contents=strdup(contents);
		}
		if (xml_tree_descend_to_child(&f_xml_tree, f_tag, f_contents, maxstr))
			f_level++;
		else
		{
			while ( (f_xml_tree) &&
					(!xml_tree_get_next_tag(f_xml_tree, f_tag, f_contents, maxstr)) )
			{
				f_level--;
				xml_tree_ascend(&f_xml_tree);
			}
		}
	}

	Xml_Write_File(output_fs, target);

	return 0;   
}


#if 1//def ZLD_GLOB_TTNET_FEATURE /* Open compiler flag "ZLD_GLOB_TTNET_FEATURE" as common at this moment, temporarily. */
                                  /* Further work will discuss the protect datamodel of TTNET into common feature. */
static int show_level = 0, op_mode = -1;

static int needToRecover(Xml_object *target, Xml_object *filter, Xml_object **keeper)
{
	int level = 0;
	int i, j;
	Xml_object *flt;
	Xml_object *level_index;

	flt = target;
	while( flt->parent )
	{
		level++;
		flt = flt->parent;
	}

	flt = filter->children;

	for( i=(level-1) ; i >= 0 ; --i)
	{
		level_index = target;
		for( j=0 ; j < i ; ++j)
			level_index = level_index->parent;

		if( flt && is_ix_style(flt) )
		{
			flt = flt->children;
			continue;
		}

		while( flt && strcmp(flt->tag, level_index->tag) )
		{
			flt = flt->nxt;
		}

		if( !flt )
		{
			/*cannot find target at target-object, means it doesn't need to recover, skip it*/
			return 0;
		}

		flt = flt->children;
	}

	if(flt)
	{
		*keeper = flt->parent;
		/*find the target at target-object, need to do recover and still have child!!so need to check child too*/
		return 2;
	}

	/*there is no child at target-object, do this node's recover*/
	return 1;
}


static int is_ix_style(Xml_object *node)
{
#if 1
	Xml_name_value_pair *attrib;
	attrib = node->attributes;

	while ( attrib ) { /*check_attribs*/
		if ( ( strcmp( attrib->name, "?" ) == 0 ) && ( attrib->value[ 0 ] == '\0' ) ) 	/* Handle special case of "?" attribute. */
			return 0;
		else  if ((strcmp(attrib->name, "instance" ) == 0)&&((strcmp(attrib->value, "x" ) == 0)||(strcmp(attrib->value, "X" ) == 0))){
			return 1;
		}
		attrib = attrib->nxt;
	}
	return 0;
#else
	if( tag[0]=='i' && (tag[1]=='x' || tag[1]=='X') )
		return 1;

	return 0;
#endif
}


static int is_iN_style(Xml_object *node)
{
#if 1
	Xml_name_value_pair *attrib;
	attrib = node->attributes;
	
	while ( attrib ) { /*check_attribs*/
		if ( ( strcmp( attrib->name, "?" ) == 0 ) && ( attrib->value[ 0 ] == '\0' ) ) 	/* Handle special case of "?" attribute. */
			return 0;
		else  if (strcmp(attrib->name, "instance" ) == 0){
			return 1;
		}
		attrib = attrib->nxt;
	}
	return 0;
#else
	char *tmp;
	int i;

	if( tag[0]=='i' && ( tag[1] >= '0' && tag[1] <= '9') )
	{
		tmp = tag+1;
		for( i=0 ; i < (strlen(tag)-1) ; ++i )
			if( !(tmp[i] >= '0' && tmp[i] <= '9') )
				break;

		if( i != strlen(tag)-1 )
			return 0;
		else
			return 1;
	}

	return 0;
#endif	
}

static void delete_node(Xml_object *node)
{
	Xml_object *tmp;
	Xml_object *previous;

	previous = tmp = node->parent->children;
	while( tmp )
	{
		if( !strcmp(tmp->tag, node->tag) )
		{
			if( previous == node->parent->children && tmp == node->parent->children)
			{
				node->parent->children = tmp->nxt;
				previous = tmp = node->parent->children;
			}
			else
			{
				previous->nxt = tmp->nxt;
				tmp = tmp->nxt;
			}
		}
		else
		{
			previous = tmp;
			tmp = tmp->nxt;
		}
	}
}


static void value_validate(Xml_object *target, Xml_object *rtobj)
{
	char *buf;
	int len;

	if( !strcmp(target->contents, rtobj->contents) )
		return;

	buf = target->contents;
	len = strlen(buf);

	if( !strcmp(target->tag, "Username") &&
		!strcmp(target->parent->parent->tag, "WANPPPConnection") )
	{
		if( !(buf[len-1]=='t' && buf[len-2]=='e' && buf[len-3]=='n' &&
			  buf[len-4]=='t' && buf[len-5]=='t' && buf[len-6]=='@') )
		{
			if( (len + strlen("@ttnet")) < XML_MAX_STRLEN )
			{
				buf[len]='@';
				buf[len+1]='t';
				buf[len+2]='t';
				buf[len+3]='n';
				buf[len+4]='e';
				buf[len+5]='t';
				buf[len+6]='\0';
			}
			else
				strcpy(target->contents, rtobj->contents);
		}
	}
}


static void fullize_value(Xml_object *rtobj, Xml_object *follower)
{
	int level = 0;
	int i, j;
	Xml_object *tracer;
	Xml_object *level_index;

	if( strcmp("", rtobj->contents) )
		return;

	tracer = rtobj;
	while( tracer->parent )
	{
		level++;
		tracer = tracer->parent;
	}
	tracer = follower;
	for( i=(level-1) ; i >= 0 ; --i)
	{
		level_index = rtobj;
		for( j=0 ; j < i ; ++j)
			level_index = level_index->parent;

		if( !tracer || !(tracer = tracer->children) )
			break;

		while( tracer && strcmp(tracer->tag, level_index->tag) )
			tracer = tracer->nxt;
	}

	if( tracer ){
		if (rtobj->contents)
			free(rtobj->contents);
		rtobj->contents = strdup(tracer->contents);
	}
}

/*[IreneWang@MSTC:TO2]$ Partial configuration and restore to default rom + for GUI backup event, 20130204 + 20130217*/
#if 1
static void do_traverse(Xml_object *target, Xml_object *rtobj,
						Xml_object *follower)
{
	Xml_object *Ttmp, *Rtmp;
	Xml_object *tmpkeeper;
	int multiRecover = 0;

	cmsLog_debug("come in do_traverse");

	Rtmp = rtobj->children;
	Ttmp = target->children;

	if( Ttmp )
	{ 						/* if target is object, then Ttmp will be the first parameter, so let's search children */
		if( !Rtmp )			/* When Rtmp is NULL */
		{ 					/* rootobj hasn't any child but target has, check do recover or not */
			int ret;
			Xml_object *keeper;
			if(op_mode == 5)
			{
				if( !(is_iN_style(Ttmp) && is_ix_style(Rtmp)) )	
					delete_node(Ttmp);	/*target is i node */
				else
					do_traverse(Ttmp, Rtmp, follower);
			}

			if( op_mode == 2 && (ret=needToRecover(Ttmp, follower, &keeper)) )
			{
				Xml_object *tmp =
				  (Xml_object*)malloc(sizeof(Xml_object));

				if(!tmp){
					printf("Fail to malloc Xml_object! (cld)\n");
					exit(1);
				}

				tmp->parent = rtobj;
				tmp->children = Ttmp->children;
				if( tmp->children )
				{ /* change pointers to rootobj's tree node */
					Xml_object *tmp_child = tmp->children;
					while(tmp_child)
					{
						tmp_child->parent = tmp;
						tmp_child = tmp_child->nxt;
					}
				}
				tmp->nxt = NULL;
				tmp->tag = Ttmp->tag;
				tmp->contents = Ttmp->contents;
				tmp->attributes = Ttmp->attributes;
				rtobj->children = tmp;

				if(ret == 2)
				{
					op_mode = 5; /* Exclusive mode */
					do_traverse(tmp, keeper, follower);
					op_mode = 2;
				}
			}
			goto skip_to_nxt;
		}

		if( !strcmp(Ttmp->tag, "ix") || !strcmp(Ttmp->tag, "iX") )	/*target is ix, need to check all root sibling*/
		{
			if( is_iN_style(Rtmp) )
			{
				if( is_iN_style(Rtmp->tag) )
				{
					show_level++;
					do_traverse( Ttmp, Rtmp, follower );
					show_level--;
				}
				Rtmp = Rtmp->nxt;
			}
		}
		else
		{
			while( Ttmp )									/*While loop if there have exist Ttmp object or Ttmp parameter*/
			{
				if(Ttmp && Rtmp)
					cmsLog_debug("First, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
				while( ( Rtmp && strcmp(Ttmp->tag, Rtmp->tag)) || multiRecover )		/*While loop if (Ttmp->tag != Rtmp->tag) or For recover multiple instance*/
				{
					if(Ttmp && Rtmp)
						cmsLog_debug("First, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
					if( !(Rtmp->nxt) )						/*true when Rtmp is last*/
					{ 									/* target's field isn't present in rootobj */
						int ret;
						Xml_object *keeper;
						if(op_mode == 5)
						{
							if( !(is_iN_style(Ttmp) && is_ix_style(Rtmp)) )
								delete_node(Ttmp);
							else
								do_traverse( Ttmp, Rtmp, follower);
						}

						if( op_mode == 2 && (ret=needToRecover(Ttmp, follower, &keeper)) )
						{
							Xml_object *tmp =
							  (Xml_object*)malloc(sizeof(Xml_object));

							if(!tmp){
								printf("Fail to malloc Xml_object! <cld>\n");
								exit(1);
							}

							tmp->parent = Rtmp->parent;
							tmp->children = Ttmp->children;
							if( tmp->children )
							{ /* change pointers to rootobj's tree node */
								Xml_object *tmp_child = tmp->children;
								while(tmp_child)
								{
									tmp_child->parent = tmp;
									tmp_child = tmp_child->nxt;
								}
							}
							tmp->nxt = NULL;
							tmp->tag = Ttmp->tag;
							tmp->contents = Ttmp->contents;
							tmp->attributes = Ttmp->attributes;

							cmsLog_debug("First, recover=%s", Ttmp->tag);
							
							if(Ttmp->children || multiRecover)
								Rtmp->nxt = tmp;
							else
							{
								Xml_object *RtmpRoot =  (Xml_object*)malloc(sizeof(Xml_object));
								if(!RtmpRoot){
									printf("Fail to malloc Xml_object! <cld>\n");
									exit(1);
								}
								RtmpRoot = Rtmp->parent;
								if (RtmpRoot->children)
									tmp->nxt = RtmpRoot->children;		/*Backup the original children*/
								RtmpRoot->children = tmp;				/*Let the recovery information(parameter) fill at the first children*/
							}

							if(ret == 2)
							{
								op_mode = 5; /* Exclusive mode */
								do_traverse(tmp, keeper, follower);
								op_mode = 2;
							}
						}
						multiRecover = 0;
						Rtmp = NULL;
						break;
					}
					Rtmp = Rtmp->nxt;
				}

				if( Rtmp )
				{ /* rootobj: child found */
					show_level++;
					if(Ttmp && Rtmp)
						cmsLog_debug("First, call do_traverse, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
					do_traverse( Ttmp, Rtmp, follower );
					if(Ttmp && Rtmp)
						cmsLog_debug("First, back do_traverse, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
					show_level--;

					if( (!(Rtmp->nxt) || !(Ttmp->nxt)) && (op_mode == 2) && (!is_iN_style(Ttmp)) && (is_iN_style(Rtmp->parent)) && (Ttmp->parent->nxt)
						&& (Rtmp->parent->nxt) && (Rtmp->parent->nxt->children) && needToRecover(Ttmp->parent, follower,&tmpkeeper))
					/*For recover multiple instance after do_traverse function only when
					1. Rtmp is last or Ttmp is last
					2. In Recover mode
					3. Ttmp is parameter and Rtmp->parent is object
					4. There exist Rtmp->parent->nxt and Rtmp->parent->nxt->children, prevent compare NULL in the later code
					5. This object (Ttmp->parent) is need to recover*/
					{	/* Prevent to access the null Ttmp->parent->nxt, 20140505 */
						if( (!strcmp(Ttmp->parent->tag, Ttmp->parent->nxt->tag)) && (!strcmp(Rtmp->parent->tag,  Rtmp->parent->nxt->tag)))
						{
							if(Ttmp && Rtmp)
								cmsLog_debug("First, prepare to jump for recover, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
							Ttmp = Ttmp->parent->nxt->children;
							target = target->nxt;
							Rtmp = Rtmp->parent->nxt->children;
							rtobj = rtobj->nxt;
						}
						else
							break;
					}
					if( (!(Rtmp->nxt) || !(Ttmp->nxt)) && (op_mode == 0) && (is_iN_style(Rtmp->parent)) && (Rtmp->parent->nxt)) 	
					{	/*For delete parameter in multiple instance, such as <Line Instance="2"><PhyReferenceList>, update on 20130902*/
						if( (!strcmp(Rtmp->parent->tag,  Rtmp->parent->nxt->tag)) && (Rtmp->parent->nxt->children))
						{
							if(Ttmp && Rtmp)
								cmsLog_debug("First, prepare to jump for delete, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
							Ttmp = Ttmp->parent->children;
							Rtmp = Rtmp->parent->nxt->children;
							rtobj = rtobj->nxt;
						}
						else
							break;
					}
					else if( (op_mode == DELETE_UNFILTER_CONFIG_MODE) && (is_iN_style(Rtmp->parent)) && (Rtmp->parent->nxt)) 	
					/*For delete_unfilter mode only when parent is multiple and there exist Rtmp->paretn->nxt, prevent loop or compare to NULL*/
					{
						if  ( !strcmp(Rtmp->parent->tag,  Rtmp->parent->nxt->tag) && ((Rtmp->parent->nxt->children) || !(Ttmp->parent->nxt->children)))
						{	/*If target's next haven't exist children, prepare to jump to next target*/
							if(Ttmp && Rtmp)
								cmsLog_debug("First, prepare to jump for unfilter, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
							if((Ttmp->children) && !(Rtmp->children))
							{	/*delete those object without children*/
								cmsLog_debug("First, delete NULL object Rtmp=%s", Rtmp->tag);
								delete_node(Rtmp);
							}
							Ttmp = Ttmp->parent->nxt->children;
							target = target->nxt;
							Rtmp = rtobj->nxt->children;
							rtobj = rtobj->nxt;
						}
						else
							break;
					}
					else
					{
						if((Ttmp->children) && (!Rtmp->children) && (op_mode == DELETE_UNFILTER_CONFIG_MODE))
						{
							cmsLog_debug("First, delete NULL object Rtmp=%s", Rtmp->tag);
							delete_node(Rtmp);
						}
						break;
					}
				}
				else
				{ /* Ttmp matches nothing, changing to next */
					if(Ttmp->nxt)		/*For multiple instance recover*/
					{
						if((!strcmp(Ttmp->tag, Ttmp->nxt->tag)) && (op_mode == 2) && needToRecover(Ttmp, follower,&tmpkeeper))
						{
							cmsLog_debug("First, multiRecover = TRUE");
							multiRecover = 1;
						}
					}
					else if( (!(Ttmp->nxt) && (target->nxt) && (rtobj->nxt) && (rtobj->nxt->children)) 
						&& (op_mode == 2) && (is_iN_style(target)) && needToRecover(target, follower,&tmpkeeper))
					{	/*For recover those has been recover before, such as <Line instance="2"><PhyReferenceList>, update on 20130902*/
						if( (!strcmp(target->tag, target->nxt->tag)) && (!strcmp(target->tag,  rtobj->nxt->tag)))
						{
							if(Ttmp)
								cmsLog_debug("First, prepare to jump for recover, Ttmp=%s", Ttmp->tag);
							Ttmp = Ttmp->parent->nxt->children;
							target = target->nxt;
							Rtmp = rtobj->nxt->children;
							rtobj = rtobj->nxt;
							continue;
						}
					}
				
					Ttmp = Ttmp->nxt;
					Rtmp = rtobj->children;

					if((!Ttmp) && (op_mode == 0))
					{
						if( (is_iN_style(Rtmp->parent)) && (Rtmp->parent->nxt) && (Rtmp->parent->nxt->children) )
						{
							if( !strcmp(Rtmp->parent->tag, Rtmp->parent->nxt->tag) )
							{
								Ttmp = target->children;
								Rtmp = rtobj->nxt->children;
								rtobj = rtobj->nxt;
								if(Ttmp && Rtmp)
									cmsLog_debug("First, let's jump, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
							}
						}
					}

				}
			}

			if( (op_mode == DELETE_UNFILTER_CONFIG_MODE) && (is_iN_style(target)) && (target->nxt) && (rtobj->nxt) 
					&& (!strcmp(target->tag,  target->nxt->tag)) && !(target->nxt->children) )
			{	/*Ignore for those null instance, such as <Line nextInstance="3" ></Line>, update on 20130902 + 20131206*/
				cmsLog_debug("First, let's ignore null instance");
				target = target->nxt;
				rtobj = rtobj->nxt;
			}
			
		}
	}
	else
	{ /* Here is the leaf node of target object */
		if((op_mode == 0) || (op_mode == DELETE_UNFILTER_CONFIG_MODE))
		{ /* DELETE NODE */
			if(rtobj->parent->parent){
				cmsLog_debug("First End, delete rtobj=%s", rtobj->tag);
				delete_node(rtobj);
			}
		}
		else if(op_mode == 1)
		{ /* value nullizing (set to null) */
			rtobj->contents[0]='\0';
		}
		else if(op_mode == 2)
		{ /* node recover */
			/***********************************
			 * Recovering back method is handled
			 * at where field's absence.
			 **********************************/
			;
		}
		else if(op_mode == 3)
		{ /* value validating */
			value_validate(target, rtobj);
		}
		else if(op_mode == 4)
		{ /* value set back if it's null */
			fullize_value(rtobj, follower);
		}
	}

skip_to_nxt:

	Ttmp = target->nxt;

	while( Ttmp )
	{ /* Search brother node (if any) */
		Rtmp = rtobj->parent->children;	
		while( ((Rtmp) && strcmp(Ttmp->tag, Rtmp->tag)) || multiRecover)		/*For recover multiple instance*/
		{
			if(Ttmp && Rtmp)
				cmsLog_debug("Second, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
			if( !(Rtmp->nxt) )
			{ /* target's field isn't present in rootobj */
				int ret;
				Xml_object *keeper;
				if(op_mode == 5)
				{
					if( !(is_iN_style(Ttmp) && is_ix_style(Rtmp)) )
						delete_node(Ttmp);
					else
						do_traverse( Ttmp, Rtmp, follower);
				}

				if( op_mode == 2 && (ret=needToRecover(Ttmp, follower,&keeper)) )
				{
					Xml_object *tmp =
					  (Xml_object*)malloc(sizeof(Xml_object));

					if(!tmp){
						printf("Fail to malloc Xml_object! <nxt>\n");
						exit(1);
					}

					tmp->parent = Rtmp->parent;
					tmp->children = Ttmp->children;
					if( tmp->children )
					{ /* change pointers to rootobj's tree node */
						Xml_object *tmp_child = tmp->children;
						while(tmp_child)
						{
							tmp_child->parent = tmp;
							tmp_child = tmp_child->nxt;
						}
					}
					tmp->nxt = NULL;
					tmp->tag = Ttmp->tag;
					tmp->contents = Ttmp->contents;
					tmp->attributes = Ttmp->attributes;

					cmsLog_debug("Second, recover=%s", Ttmp->tag);

					if((Ttmp->children) || multiRecover)
						Rtmp->nxt = tmp;
					else
					{
						Xml_object *RtmpRoot =  (Xml_object*)malloc(sizeof(Xml_object));
						if(!RtmpRoot){
							printf("Fail to malloc Xml_object! <cld>\n");
							exit(1);
						}
						RtmpRoot = Rtmp->parent;
						if (RtmpRoot->children)
							tmp->nxt = RtmpRoot->children;		/*Backup the original children*/
						RtmpRoot->children = tmp;				/*Let the recovery information(parameter) fill at the first children*/
					}

					if(ret == 2)
					{
						op_mode = 5;
						do_traverse(tmp, keeper, follower);
						op_mode = 2;
					}
				}
				multiRecover = 0;		/*reset the multiple instance recover tag*/
				Rtmp = NULL;
				break;
			}
			Rtmp = Rtmp->nxt;
		}

		if( Rtmp )
		{ /* rootobj: brother node found */
			if(Ttmp && Rtmp)
				cmsLog_debug("Second, call do_traverse, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
			do_traverse( Ttmp, Rtmp, follower );
			if(Ttmp && Rtmp)
				cmsLog_debug("Second, back do_traverse, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);

			if( (!(Rtmp->nxt) || !(Ttmp->nxt)) && ((op_mode == 2) || (op_mode == 0) ||(op_mode == DELETE_UNFILTER_CONFIG_MODE)) 
				&& (is_iN_style(Rtmp->parent))	&& (Rtmp->parent->nxt)) 	
			/*For multiple instance only when 
				1. Rtmp is last or Ttmp is last
				2. Mode is recover or delete
				3. There exist Rtmp->parent and Rtmp->paretn->nxt, prevent compare to NULL*/
			{
				if( (op_mode == 0) && ( !strcmp(Rtmp->parent->tag,  Rtmp->parent->nxt->tag)) && (Rtmp->parent->nxt->children))
				{
					if(Ttmp && Rtmp)
						cmsLog_debug("Second, prepare to jump for delete, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
					Ttmp = Ttmp->parent->children;
					rtobj = Rtmp->parent->nxt->children;
				}
				else if ( (op_mode == DELETE_UNFILTER_CONFIG_MODE) && ( !strcmp(Rtmp->parent->tag,  Rtmp->parent->nxt->tag)) && ((Rtmp->parent->nxt->children) || !(Ttmp->parent->nxt->children)))
				{
					if(Ttmp && Rtmp)
						cmsLog_debug("Second, prepare to jump for unfilter, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
					rtobj = Rtmp->parent->nxt->children;
					if((Ttmp->children) && !(Rtmp->children))
					{
						cmsLog_debug("Second, delete NULL object Rtmp=%s", Rtmp->tag);
						delete_node(Rtmp);
					}
					Ttmp = Ttmp->parent->nxt->children;
				}
				else if( (op_mode == 2 ) && (Rtmp->parent->nxt->children) && needToRecover(Ttmp->parent, follower,&tmpkeeper) 
					&& (!is_iN_style(Ttmp)) && ((!strcmp(Ttmp->parent->tag, Ttmp->parent->nxt->tag)) && (!strcmp(Rtmp->parent->tag,  Rtmp->parent->nxt->tag))) )
				{
					if(Ttmp && Rtmp)
						cmsLog_debug("Second, prepare to jump for recover, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
					Ttmp = Ttmp->parent->nxt->children;
					rtobj = Rtmp->parent->nxt->children;
				}
				else
				{
					if((Ttmp->children) && !(Rtmp->children) && (op_mode == DELETE_UNFILTER_CONFIG_MODE))
					{
						cmsLog_debug("Second, delete NULL object Rtmp=%s", Rtmp->tag);
						delete_node(Rtmp);
					}
					break;
				}
			}
			else
			{
				if((Ttmp->children) && !(Rtmp->children) && (op_mode == DELETE_UNFILTER_CONFIG_MODE))
				{
					cmsLog_debug("Second, delete NULL object Rtmp=%s", Rtmp->tag);
					delete_node(Rtmp);
				}
				break;
			}
		}
		else
		{ /* This field matches nothing, changing to next */
			if(Ttmp->nxt)		/*For multiple instance recover*/
			{
				if((!strcmp(Ttmp->tag, Ttmp->nxt->tag)) && (op_mode == 2) && needToRecover(Ttmp, follower,&tmpkeeper))
				{
					cmsLog_debug("Second, multiRecover = TRUE");
					multiRecover = 1;
				}
			}
	
			Ttmp = Ttmp->nxt;

			if((!Ttmp) && ((op_mode == 0) || (op_mode == DELETE_UNFILTER_CONFIG_MODE)))
			{
				if( (is_iN_style(rtobj->parent)) && (rtobj->parent->nxt) && (rtobj->parent->nxt->children) )
				{
					if( !strcmp(rtobj->parent->tag,  rtobj->parent->nxt->tag) )
					{
						if(Ttmp && Rtmp)
							cmsLog_debug("Second : prepare to jump, Ttmp=%s, Rtmp=%s", Ttmp->tag, Rtmp->tag);
						if (op_mode == 0)
						{
							Ttmp = target->parent->children;
							rtobj = rtobj->parent->nxt->children;
						}
						else if (op_mode == DELETE_UNFILTER_CONFIG_MODE)
						{
							Ttmp = target->parent->nxt->children;
							rtobj = rtobj->parent->nxt->children;
						}
					}
				}
			}
		}
	}
	cmsLog_debug("leave do_traverse");
}
#else
static void do_traverse(Xml_object *target, Xml_object *rtobj,
						Xml_object *follower)
{
	Xml_object *Ttmp, *Rtmp;

	Rtmp = rtobj->children;
	Ttmp = target->children;

	if( Ttmp )
	{ /* Search children (if any) */
		if( !Rtmp )
		{ /* rootobj: hasn't any child but target has */
			int ret;
			Xml_object *keeper;
			if(op_mode == 5)
			{
				if( !(is_iN_style(Ttmp->tag) && is_ix_style(Rtmp->tag)) )
					delete_node(Ttmp);
				else
					do_traverse(Ttmp, Rtmp, follower);
			}

			if( op_mode == 2 && (ret=needToRecover(Ttmp, follower, &keeper)) )
			{
				Xml_object *tmp =
				  (Xml_object*)malloc(sizeof(Xml_object));

				if(!tmp){
					printf("Fail to malloc Xml_object! (cld)\n");
					exit(1);
				}

				tmp->parent = rtobj;
				tmp->children = Ttmp->children;
				if( tmp->children )
				{ /* change pointers to rootobj's tree node */
					Xml_object *tmp_child = tmp->children;
					while(tmp_child)
					{
						tmp_child->parent = tmp;
						tmp_child = tmp_child->nxt;
					}
				}
				tmp->nxt = NULL;
				tmp->tag = Ttmp->tag;
				tmp->contents = Ttmp->contents;
				tmp->attributes = Ttmp->attributes;
				rtobj->children = tmp;

				if(ret == 2)
				{
					op_mode = 5; /* Exclusive mode */
					do_traverse(tmp, keeper, follower);
					op_mode = 2;
				}
			}
			goto skip_to_nxt;
		}

		if( !strcmp(Ttmp->tag, "ix") || !strcmp(Ttmp->tag, "iX") )
		{
			while( Rtmp )
			{
				if( is_iN_style(Rtmp->tag) )
				{
					show_level++;
					do_traverse( Ttmp, Rtmp, follower );
					show_level--;
				}
				Rtmp = Rtmp->nxt;
			}
		}
		else
		{
			while( Ttmp )
			{
				while( strcmp(Ttmp->tag, Rtmp->tag) )
				{
					if( !(Rtmp->nxt) )
					{ /* target's field isn't present in rootobj */
						int ret;
						Xml_object *keeper;
						if(op_mode == 5)
						{
							if( !(is_iN_style(Ttmp->tag) && is_ix_style(Rtmp->tag)) )
								delete_node(Ttmp);
							else
								do_traverse( Ttmp, Rtmp, follower);
						}

						if( op_mode == 2 && (ret=needToRecover(Ttmp, follower, &keeper)) )
						{
							Xml_object *tmp =
							  (Xml_object*)malloc(sizeof(Xml_object));

							if(!tmp){
								printf("Fail to malloc Xml_object! <cld>\n");
								exit(1);
							}

							tmp->parent = Rtmp->parent;
							tmp->children = Ttmp->children;
							if( tmp->children )
							{ /* change pointers to rootobj's tree node */
								Xml_object *tmp_child = tmp->children;
								while(tmp_child)
								{
									tmp_child->parent = tmp;
									tmp_child = tmp_child->nxt;
								}
							}
							tmp->nxt = NULL;
							tmp->tag = Ttmp->tag;
							tmp->contents = Ttmp->contents;
							tmp->attributes = Ttmp->attributes;
							Rtmp->nxt = tmp;

							if(ret == 2)
							{
								op_mode = 5; /* Exclusive mode */
								do_traverse(tmp, keeper, follower);
								op_mode = 2;
							}
						}
						Rtmp = NULL;
						break;
					}
					Rtmp = Rtmp->nxt;
				}

				if( Rtmp )
				{ /* rootobj: child found */
					show_level++;
					do_traverse( Ttmp, Rtmp, follower );
					show_level--;
					break;
				}
				else
				{ /* Ttmp matches nothing, changing to next */
					Ttmp = Ttmp->nxt;
					Rtmp = rtobj->children;
				}
			}
		}
	}
	else
	{ /* Here is the leaf node of target object */
		if(op_mode == 0)
		{ /* DELETE NODE */
			if(rtobj->parent->parent){
				delete_node(rtobj);
			}
		}
		else if(op_mode == 1)
		{ /* value nullizing (set to null) */
			rtobj->contents[0]='\0';
		}
		else if(op_mode == 2)
		{ /* node recover */
			/***********************************
			 * Recovering back method is handled
			 * at where field's absence.
			 **********************************/
			;
		}
		else if(op_mode == 3)
		{ /* value validating */
			value_validate(target, rtobj);
		}
		else if(op_mode == 4)
		{ /* value set back if it's null */
			fullize_value(rtobj, follower);
		}
	}

skip_to_nxt:

	Ttmp = target->nxt;

	while( Ttmp )
	{ /* Search brother node (if any) */
		Rtmp = rtobj->parent->children;
		while( strcmp(Ttmp->tag, Rtmp->tag) )
		{
			if( !(Rtmp->nxt) )
			{ /* target's field isn't present in rootobj */
				int ret;
				Xml_object *keeper;
				if(op_mode == 5)
				{
					if( !(is_iN_style(Ttmp->tag) && is_ix_style(Rtmp->tag)) )
						delete_node(Ttmp);
					else
						do_traverse( Ttmp, Rtmp, follower);
				}

				if( op_mode == 2 && (ret=needToRecover(Ttmp, follower,&keeper)) )
				{
					Xml_object *tmp =
					  (Xml_object*)malloc(sizeof(Xml_object));

					if(!tmp){
						printf("Fail to malloc Xml_object! <nxt>\n");
						exit(1);
					}

					tmp->parent = Rtmp->parent;
					tmp->children = Ttmp->children;
					if( tmp->children )
					{ /* change pointers to rootobj's tree node */
						Xml_object *tmp_child = tmp->children;
						while(tmp_child)
						{
							tmp_child->parent = tmp;
							tmp_child = tmp_child->nxt;
						}
					}
					tmp->nxt = NULL;
					tmp->tag = Ttmp->tag;
					tmp->contents = Ttmp->contents;
					tmp->attributes = Ttmp->attributes;
					Rtmp->nxt = tmp;

					if(ret == 2)
					{
						op_mode = 5;
						do_traverse(tmp, keeper, follower);
						op_mode = 2;
					}
				}
				Rtmp = NULL;
				break;
			}
			Rtmp = Rtmp->nxt;
		}

		if( Rtmp )
		{ /* rootobj: brother node found */
			do_traverse( Ttmp, Rtmp, follower );
			break;
		}
		else
		{ /* This field matches nothing, changing to next */
			Ttmp = Ttmp->nxt;
		}
	}
}
#endif  /*1*/

int traverse_tree( Xml_object *rootobj, Xml_object *target,Xml_object *follower, FILE* output_fs)
{
	/* Mainly Go through with "target";
	   finally to write with "rootobj". */

	show_level = 0;

	if(!follower)
		follower = rootobj;
	do_traverse(target, rootobj, follower);

	if( op_mode == 3 )
		rootobj = target;

	Xml_Write_File( output_fs, rootobj );

	return 0;
}
#endif /* ZLD_GLOB_TTNET_FEATURE */


int read_file (FILE * Read, char** buf)
{
	int c = 0;
	int CharInFile=0;
      
  do {
        c = 0;
        c = fgetc (Read);
        if (c != EOF) {       
            *buf = (char *) realloc (*buf, (CharInFile + 1) * sizeof(char));
            (*buf)[CharInFile] = c;
            CharInFile++;
        } 
  } while (c != EOF); 
  	       
  return CharInFile;
}


#if 1
unsigned int FilterUnconfig(char *buf, unsigned int len, unsigned int *filtedlen){
	FILE *in_file=NULL, *out_file= NULL, *filter_file=NULL, *delete_file=NULL;
	int count=0;
	int fileLen=0;

	Xml_object *rootobj = 0;
	Xml_object *filter_obj = 0;
	Xml_object *out_obj = 0;

	FILE *tmp_file=NULL;
	Xml_object *tmp_obj=NULL;

	cmsLog_debug("here in FilterUnconfig");

	in_file = fopen(XML_INPUT_FILE,"w");
	count = fwrite(buf, 1, len, in_file);  
	if(count!=len){
		printf("Write %s file fail!\n",XML_INPUT_FILE);
		/* Fix coverity [Returned without freeing storage]*/
		if(in_file){
			fclose(in_file);
		}
		return -1;
	}
	
	in_file = freopen(XML_INPUT_FILE,"r",in_file);
	if(in_file != NULL)
	{
		rootobj = Xml_Read_File(in_file);
		if(rootobj == 0)
		{
			 printf("rootobj is null\n");
			 /*Fix coverity [Returned without freeing storage]*/
			 if(in_file){
				fclose(in_file);
			 }
			 return -1;
		}
	}

	filter_file = fopen(XML_GUIPARTIAL_RESET_FILE,"r");
	if(filter_file!=NULL){
		filter_obj = Xml_Read_File(filter_file);	
		if(filter_obj == 0)
		{
			 printf("filter_obj is null\n");
			 fclose(filter_file);
			 return -1;
		}
		fclose(filter_file);
	}

	delete_file = fopen(XML_DELETE_FILE,"w");			/*Preserve the unfilter part from input xml*/
	if(config_proc(DELETE_CONFIG_MODE,rootobj,filter_obj,NULL,delete_file)==-1){
		printf("config_proc DELETE_CONFIG_MODE fail \n");
		fclose(delete_file);
		return -1;
	}
	
	cmsLog_debug("after first delete");

	in_file = freopen(XML_INPUT_FILE,"r",in_file);
	if(in_file != NULL)
	{
		rootobj = Xml_Read_File(in_file);
		cmsLog_debug("read the correct input file");
		if(rootobj == 0)
		{
			 printf("rootobj is null\n");
			 /*Fix coverity [Returned without freeing storage]*/
			 if(in_file){
				fclose(in_file);	printf("close in_file\n");
			 }
			 return -1;
		}
		fclose(in_file);
	}

	delete_file = freopen(XML_DELETE_FILE,"r",delete_file);
	if (delete_file != NULL)
	{
		filter_obj = Xml_Read_File(delete_file);				/*Now the filter = rootobj - filter = unfilter*/
		cmsLog_debug("read the correct delete file");
		if(filter_obj == 0)
		{
			 printf("filter_obj is null\n");
			 /*Fix coverity [Returned without freeing storage]*/
			 if(delete_file){
				fclose(delete_file);
			 }
			 return -1;
		}
		fclose(delete_file);
	}
		
	out_file = fopen(XML_OUTPUT_FILE,"w");						/*Now the rootobj = rootobj - !filter*/
	if(config_proc(DELETE_UNFILTER_CONFIG_MODE,rootobj,filter_obj,NULL,out_file)!=-1){
		out_file = freopen(XML_OUTPUT_FILE,"r",out_file);
	  	fileLen=fread(buf, 1 ,len , out_file);					
		if(fileLen > 0){
			*filtedlen=fileLen;
		}
	}
	
	if(out_file){
		fclose(out_file);
	}

	unlink(XML_INPUT_FILE);
	unlink(XML_DELETE_FILE);
/*	unlink(XML_OUTPUT_FILE);	leave the file for debug in each backup, update on 20130902*/
	cmsLog_debug("leave FilterUnconfig success");

	return 0;

}
#else
unsigned int FilterUnconfig(char *buf, unsigned int len, unsigned int *filtedlen){	/*irene test*/
	FILE *in_file=NULL, *out_file= NULL, *filter=NULL;
	int count=0;
	int fileLen=0;

	Xml_object *rootobj = 0;
	Xml_object *filter_obj = 0;

	in_file = fopen(XML_INPUT_FILE,"w");
	count = fwrite(buf, 1, len, in_file);  
	if(count!= len){
		printf("Write %s file fail!\n",XML_INPUT_FILE);
		/*Fix coverity [Returned without freeing storage]*/
		if(in_file){
			fclose(in_file);
		}
		return -1;
	}
	
	in_file = freopen(XML_INPUT_FILE,"r",in_file);
	rootobj = Xml_Read_File(in_file);
	if(rootobj == 0)
	{
		 printf("rootobj is null\n");
		 /*Fix coverity [Returned without freeing storage]*/
		 if(in_file){
			fclose(in_file);
		 }
		 return -1;
	}
	
	if(in_file){
		fclose(in_file);
	}

	if(partial == 1)				/*irene test*/
		filter = fopen(XML_ACSPARTIAL_RESET_FILE,"r");
	if(filter!=NULL){
		filter_obj = Xml_Read_File(filter);	
		if(filter_obj == 0)
		{
			 printf("filter_obj is null\n");
			 fclose(filter);
			 return -1;
		}
		fclose(filter);
	}

	out_file = fopen(XML_OUTPUT_FILE,"w");	
	if(config_proc(DELETE_CONFIG_MODE,rootobj,filter_obj,NULL,out_file)!=-1){
		out_file = freopen(XML_OUTPUT_FILE,"r",out_file);
	  	fileLen=fread(buf, 1 ,len , out_file);					
		if(fileLen > 0){
			*filtedlen=fileLen;
		}
	}
	if(out_file){
		fclose(out_file);
	}

	unlink(XML_INPUT_FILE);
	unlink(XML_OUTPUT_FILE);

	return 0;

}
#endif  /*1*/

unsigned int Mergeconfig(char *newbuf, char *oribuf, unsigned int newlen, unsigned int orilen ,char **merge_body ,unsigned int *mergedlen, unsigned int partial){	/*irene test*/

/*[IreneWang@MSTC:TO2]$ Partial configuration and restore + partial restore to default rom + 
	The output config file must be encrypted, 20121227 + 20130204 + 20130228*/
#if 1

	FILE *default_file=NULL, *orignal_file=NULL,*filter_file=NULL;
	FILE *delete_file=NULL;				/*For original - filter*/
	FILE *out_file= NULL;					/* For (original-filter) + (filter with default rom value)*/
	unsigned int fileLen=0, count=0;
	char *tmpbuf=NULL;

	Xml_object *filter_obj = NULL;				/*Define by case, and it can choose different filter by using different partial parameter*/
	Xml_object *orignal_obj = NULL;			/*Equal to the backup setting*/
	Xml_object *default_obj = NULL;			/*Equal to the default rom file (ex: 132VQH.rom) */

	cmsLog_debug("irene : You have been success in the Mergeconfig function, partial=%d", partial);

	orignal_file = fopen(XML_ORIGNAL_FILE,"w");
	count = fwrite(oribuf, 1, orilen, orignal_file);
	if(count!= orilen){
	        printf("Write %s file fail!\n",XML_ORIGNAL_FILE);
	        return -1;
	}

	orignal_file = freopen(XML_ORIGNAL_FILE,"r",orignal_file);
	if(orignal_file){
		orignal_obj = Xml_Read_File(orignal_file);
		if(orignal_obj == 0)
		{
			 printf("orignal_obj is null\n");
			 fclose(orignal_file);
			 return -1;
		}		
		fclose(orignal_file);
	}

	if(partial == CMS_IMAGE_FORMAT_ACSXML_CFG)
		filter_file = fopen(XML_ACSPARTIAL_RESET_FILE,"r");
	else if(partial == CMS_IMAGE_FORMAT_GUIXML_CFG)
		filter_file = fopen(XML_GUIPARTIAL_RESET_FILE,"r");
	else if(partial == CMS_IMAGE_FORMAT_AREAXML_CFG)
		filter_file = fopen(XML_AREAPARTIAL_RESET_FILE,"r");

	if(filter_file != NULL){
		filter_obj = Xml_Read_File(filter_file);
		if(filter_obj == 0)
		{
			 printf("filter_obj is null\n");
			 fclose(filter_file);
			 return -1;
		}
		fclose(filter_file);
	}

	cmsLog_debug("irene : welcome to  delete mode.");
	
	delete_file = fopen(XML_DELETE_FILE,"w");			/*Delete the filter part from original xml*/
	if(config_proc(DELETE_CONFIG_MODE,orignal_obj,filter_obj,NULL,delete_file)==-1){
		printf("config_proc DELETE_CONFIG_MODE fail \n");
		fclose(delete_file);
		return -1;
	}

	delete_file = freopen(XML_DELETE_FILE,"r",delete_file);
	orignal_obj = Xml_Read_File(delete_file);				/*Now the original = backupsetting(original) - filter */

	if(newbuf)
	{
		default_file = fopen(XML_INPUT_FILE,"w");
		count = fwrite(newbuf, 1, newlen, default_file);  
		if(count!= newlen){
			printf("Write %s file fail!\n",XML_INPUT_FILE);
			return -1;
		}
		default_file = freopen(XML_INPUT_FILE,"r",default_file);
		cmsLog_debug("irene: here is GUI partial update.");
	}
	else
	{
		default_file = fopen(XML_DEFAULT_FILE,"r");
	}

	if(default_file){
		default_obj = Xml_Read_File(default_file);
		if(default_obj == 0)
		{
		 	printf("default_obj is null\n");
			fclose(default_file);
		 	return -1;
		}
		fclose(default_file);
	}

	cmsLog_debug("irene : welcome to  recover mode.");
	
	out_file = fopen(XML_OUTPUT_FILE,"w");				/*out_file = original + filter with default value*/
		/*recover unconfigure items from original configuration to default configuration file*/
	if(config_proc(RECOVER_CONFIG_MODE,default_obj,filter_obj,orignal_obj,out_file)!=-1){		 
		out_file = freopen(XML_OUTPUT_FILE,"r",out_file);		
		fileLen=read_file(out_file,&tmpbuf);
		*merge_body=tmpbuf;
		*mergedlen=fileLen;
	}

	if(out_file){
		fclose(out_file);
	}
	if(delete_file){
		fclose(delete_file);
	}

	unlink(XML_OUTPUT_FILE);		/*here will remove the file, if you want to double check the final file, you can mark the unlink*/
	unlink(XML_DELETE_FILE);	
	unlink(XML_ORIGNAL_FILE);

	cmsLog_debug("irene : You will leave Mergeconfig function sucessfully");

#else	/* xml_parse_lib's default mergeconfig*/

	in_file = fopen(XML_INPUT_FILE,"w");	
	count = fwrite(newbuf, 1, newlen, in_file);  
	if(count!= newlen){
		printf("Write %s file fail!\n",XML_INPUT_FILE);
		return -1;
	}
	
	in_file = freopen(XML_INPUT_FILE,"r",in_file);
	newobj = Xml_Read_File(in_file);
	if(newobj == 0)
	{
		 printf("rootobj is null\n");
		 return -1;
	}
	
	if(in_file){
		fclose(in_file);
	}

	orignal_file = fopen(XML_ORIGNAL_FILE,"w");
	count = fwrite(oribuf, 1, orilen, orignal_file);  
	if(count!= orilen){
		printf("Write %s file fail!\n",XML_ORIGNAL_FILE);
		return -1;
	}
	
	orignal_file = freopen(XML_ORIGNAL_FILE,"r",orignal_file);
	if(orignal_file){
		orignal_obj = Xml_Read_File(orignal_file);
		if(orignal_obj == 0)
		{
			 printf("orignal_obj is null\n");
			 return -1;
		}		
		fclose(orignal_file);
	}

	filter = fopen(XML_UNCONFIG_FILTER_FILE,"r");
	if(filter!=NULL){
		filter_obj = Xml_Read_File(filter);	
		if(filter_obj == 0)
		{
			 printf("filter_obj is null\n");
			 fclose(filter);
			 return -1;
		}
		fclose(filter);
	}	

	delete_file = fopen(XML_DELETE_FILE,"w");	
	if(config_proc(DELETE_CONFIG_MODE,newobj,filter_obj,NULL,delete_file)==-1){		
		printf("config_proc DELETE_CONFIG_MODE fail \n");
		fclose(delete_file);
		return -1;
	}	

	delete_file = freopen(XML_DELETE_FILE,"r",delete_file);	
	newobj=Xml_Read_File(delete_file);
	
	out_file = fopen(XML_OUTPUT_FILE,"w");	
	if(config_proc(RECOVER_CONFIG_MODE,orignal_obj,filter_obj,newobj,out_file)!=-1){				
		out_file = freopen(XML_OUTPUT_FILE,"r",out_file);		
		fileLen=read_file(out_file,&tmpbuf);
		*merge_body=tmpbuf;
		*mergedlen=fileLen;
	}
	
	if(out_file){
		fclose(out_file);
	}
	
	if(delete_file){
		fclose(delete_file);
	}

	

	unlink(XML_INPUT_FILE);
	unlink(XML_OUTPUT_FILE);
	unlink(XML_DELETE_FILE);
	unlink(XML_ORIGNAL_FILE);

#endif  /*1*/
	
	return 0;
}



int config_proc(int mode,Xml_object *rootobj,Xml_object *filter_obj,Xml_object *pro_obj, FILE * output_fs){

	int result=-1;	

	switch(mode){
		case DUMP_CONFIG_MODE:
			if(filter_obj == 0){
				return -1;
			}
			result = filter_password(rootobj, filter_obj, output_fs);
			break;	
		case MERGE_CONFIG_MODE:
			if(filter_obj == 0){
				return -1;
			}
			if(pro_obj == 0){
				return -1;
			}
			result = merge_password(rootobj, pro_obj, filter_obj, output_fs);
			break;
			
		case DELETE_CONFIG_MODE:
			if(filter_obj == 0){
				return -1;
			}
			op_mode = 0;
			result = traverse_tree(rootobj, filter_obj, NULL, output_fs);
			break;
		case NULLIZE_CONFIG_MODE:
			if(filter_obj == 0){
				return -1;
			}
			op_mode = 1;
			result = traverse_tree(rootobj, filter_obj, NULL, output_fs);
			break;
		case RECOVER_CONFIG_MODE:
			if(filter_obj == 0){
				return -1;
			}
			if(pro_obj == 0){
				return -1;
			}
			op_mode = 2;
			result = traverse_tree(pro_obj, rootobj, filter_obj, output_fs);
			break;
		case VALIDATE_CONFIG_MODE:
			if(pro_obj == 0){
				return -1;
			}
			op_mode = 3;
			result = traverse_tree(rootobj, pro_obj, NULL, output_fs);
			break;
		case FULLIZE_CONFIG_MODE:
			if(filter_obj == 0){
				return -1;
			}
			if(pro_obj == 0){
				return -1;
			}
			op_mode = 4;
			result = traverse_tree(pro_obj, filter_obj, rootobj, output_fs);
			break;
		case DELETE_UNFILTER_CONFIG_MODE:
			if(filter_obj == 0){
				return -1;
			}
			op_mode = DELETE_UNFILTER_CONFIG_MODE;
			result = traverse_tree(rootobj, filter_obj, NULL, output_fs);
			break;
		default:
			printf("Please input correct mode!\n");
			break;
	}

	return result;
}
