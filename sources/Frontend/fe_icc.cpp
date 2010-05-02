/**
 * @file: fe_icc.cpp
 * Implementation of ICC Frontend
 */
/* 
 * Frontend in ShowGraph tool.
 * Copyright (C) 2009  Boris Shurygin
 */
#include "fe_iface.h"


/** Check if this line starts a new node text section */
bool
TestParser::nodeStartIL0( QString line)
{
    return line.indexOf("BBLOCK") != -1;
}
/** Check whether the given line should be treated as a next line */
bool
TestParser::nextLineIL0( QString line)
{
    return true;
}

void
TestParser::parseLineIL0( QString line)
{
    QString n_str("Node ");
    QString e_str("Edge ");
    /** Node recognition */
    QRegExp node_rx("BBLOCK (\\d+)");
    QRegExp preds_rx("^preds:");
    QRegExp succs_rx("^succs:");
    QTextStream stream( stdout);

            
    /** Expression recognition */
    int pos = 0;

    if ( preds_rx.indexIn( line) == 0)
    {
        int index = preds_rx.matchedLength();
        QRegExp num_rx(" (\\d+)");
        while ( ( index = num_rx.indexIn( line, index) )!= -1)
        {
            index += num_rx.matchedLength();
            
            assert( isNotNullP( curr_node));
            int pred_num =  num_rx.cap(1).toInt();
            QString name = QString("%1->%2")
                           .arg( pred_num)
                           .arg( curr_node->irId());
            QString pred_name = QString("Node %1").arg( pred_num);
            QString succ_name = QString("Node %1").arg( curr_node->irId());
            
            /** Add edge to symtab */
            if ( symtab.find( name) == symtab.end() 
                 && symtab.find( pred_name) != symtab.end() 
                 && symtab.find( succ_name) != symtab.end())
            {
                SymEdge *edge = new SymEdge( name);
                edge->setPred( pred_name);
                edge->setSucc( succ_name);
                symtab[ name] = edge;

                /** Add edge to graph */
                GNode* pred = static_cast< SymNode *>( symtab[ pred_name])->node();
                GNode* succ = static_cast< SymNode *>( symtab[ succ_name])->node();
                GEdge* e = graph->graph()->newEdge( pred, succ);
    #ifdef _DEBUG            
                //stream << name << ": " << pred_name << "->" << succ_name << endl;
    #endif      
            }
        }
    } else if ( succs_rx.indexIn( line) == 0)
    {
        int index = succs_rx.matchedLength();
        QRegExp num_rx(" (\\d+)");
        while ( ( index = num_rx.indexIn( line, index) )!= -1)
        {
            index += num_rx.matchedLength();
            assert( isNotNullP( curr_node));
            int pred_num =  num_rx.cap(1).toInt();
            QString name = QString("%1->%2")
                           .arg( curr_node->irId())
                           .arg( pred_num);
            QString pred_name = QString("Node %1").arg( curr_node->irId());
            QString succ_name = QString("Node %1").arg( pred_num);
            
            /** Add edge to symtab */
            if ( symtab.find( name) == symtab.end() 
                 && symtab.find( pred_name) != symtab.end() 
                 && symtab.find( succ_name) != symtab.end())
            {
                SymEdge *edge = new SymEdge( name);
                edge->setPred( pred_name);
                edge->setSucc( succ_name);
                symtab[ name] = edge;

                /** Add edge to graph */
                GNode* pred = static_cast< SymNode *>( symtab[ pred_name])->node();
                GNode* succ = static_cast< SymNode *>( symtab[ succ_name])->node();
                GEdge* e = graph->graph()->newEdge( pred, succ);
    #ifdef _DEBUG            
                //stream << name << ": " << pred_name << "->" << succ_name << endl;
    #endif      
            }
        }
    } else if ( node_rx.indexIn( line) != -1 )
    {
		bool good_id = false;
		int ir_id = node_rx.cap(1).toInt( &good_id);
		QString text = QString("BBLOCK ").append( node_rx.cap(1));
        QString name = n_str.append( node_rx.cap(1));
        
        /** Add node to symtab */
        if ( symtab.find( name ) == symtab.end())
        {
            SymNode* node = new SymNode( name);
            curr_node = static_cast<CFNode *>( graph->graph()->newNode());
            curr_node->setDoc( new QTextDocument());
            node->setNode( curr_node);
            node->node()->item()->setPlainText( text);
		    if ( good_id)
		    {
			    node->node()->setIRId( ir_id);
		    }
            symtab[ name] = node;
#ifdef _DEBUG
            //stream << name << endl;
#endif
        }
    } else
    {
        if ( !isStateNode())
            setStateDefault();
    }
    if ( isStateNode())
    {
        node_text.append( line).append( "\n");
    }
}

/** Parse IL0 dump */
void TestParser::parseIL0Unit( DumpUnitInfo *unit)
{
    total_lines_num = 0;
    DumpPos beg = unit->pos();
    DumpPos end = unit->end();
    if ( !file.isOpen() && !file.open( QIODevice::ReadOnly))
    {
        return;
    }
    file.reset();
    /** Read file line by line */
    QTextStream in( &file);
    QString line;
    unit_beg = 0;
    unit_id = 0;
    do
    {
        line = in.readLine();
        if ( total_lines_num >= beg)
        {
            break;  
        }
        total_lines_num++;
    } while ( !line.isNull());
    cur_line_num = 0;

    /** Init state */
    setStateInit();
#ifdef _DEBUG
        out( "Started parsing");
#endif
    do
    {
        curr_line = in.readLine();
        cur_line_num++;
        if ( !nextLineIL0( curr_line))
        {
            line.append( curr_line);
        } else
        {
            if ( !line.isNull())
            {
                if ( nodeStopIL0( line))
                {
                    endNode();
                    setStateDefault();
                }
                if ( nodeStartIL0( line))
                {
                    setStateNode();
                    startNode();
                }             
                parseLineIL0( line);
            }
            line = curr_line;
        }
        if ( total_lines_num >= end)
            break;
        total_lines_num++;
    } while ( !curr_line.isNull());
    if ( isStateNode())
    {
        endNode();
    }
#ifdef _DEBUG
        out( "Finished parsing");
#endif
}