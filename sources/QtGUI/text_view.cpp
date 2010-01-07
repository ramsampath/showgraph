/**
 * @file: text_view.cpp
 * TextView class implementation
 */
/*
 * GUI for ShowGraph tool.
 * Copyright (C) 2009  Boris Shurygin
 */
#include "gui_impl.h"

/**
 * Highlight given text
 */
void DumpHighlighter::highlightBlock(const QString &text)
{
     QTextCharFormat myClassFormat;
     myClassFormat.setFontWeight(QFont::Bold);
     myClassFormat.setForeground(Qt::darkMagenta);
	 myClassFormat.setAnchor( true);

	 QRegExp exp("Node (\\d+)");

	 int index = text.indexOf( exp);
     while (index >= 0)
	 {
         int length = exp.matchedLength();
         
		 myClassFormat.setAnchorHref( exp.cap( 1));
		 setFormat(index, length, myClassFormat);
         
		 index = text.indexOf( exp, index + length);
     }
}

/**
 * TextView Constructor
 */
TextView::TextView(): hl( NULL)
{
	setOpenLinks( false);
}

/**
 * Highlight text
 */
void TextView::highlighText()
{
	//hl = new DumpHighlighter( document());
	 QTextCursor cursor( document());
	 QString text = document()->toPlainText();
	 QRegExp exp("Node (\\d+)");
	 int index = text.indexOf( exp);
     while (index >= 0)
	 {
         int length = exp.matchedLength();
         
		 QTextCharFormat myClassFormat;
		 //myClassFormat.setFontWeight(QFont::Bold);
		 myClassFormat.setForeground(Qt::blue);
		 myClassFormat.setFontUnderline( true);
		 myClassFormat.setAnchor( true);
		 myClassFormat.setAnchorHref( exp.cap( 1));
		 cursor.setPosition( index);
		 cursor.setPosition( index + length, QTextCursor::KeepAnchor);
		 cursor.removeSelectedText();
		 cursor.insertText( QString( "Node %1").arg( exp.cap( 1)), myClassFormat);
		 index = text.indexOf( exp, index + length);
     }
}

void
TextView::openFile( QString fileName)
{
     if ( !QFile::exists( fileName))
         return;

     QFile file( fileName);
     if ( !file.open(QFile::ReadOnly))
         return;

     QByteArray data = file.readAll();
     QTextCodec *codec = Qt::codecForHtml(data);
     QString str = codec->toUnicode(data);
     
     if ( Qt::mightBeRichText(str))
     {
         setHtml(str);
     } else
     {
         str = QString::fromLocal8Bit(data);
         setPlainText(str);
     }
}

void TextView::mousePressEvent( QMouseEvent * mouseEvent)
{
	QTextBrowser::mousePressEvent( mouseEvent);
}

/**
 * TextView destructor
 */
TextView::~TextView()
{
	delete hl;
}
