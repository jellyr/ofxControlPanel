#include "guiTypeFileLister.h"


//TODO: fix this!


//TODO: fix bitmap type specific code

//------------------------------------------------
guiTypeFileLister::guiTypeFileLister(){
	lister			= NULL;
	pct				= 0;
	sliderWidth		= 20;
	selectPct		= -1;
	selection		= -1;
	selectionTmp	= -1;
	startPos		= 0;
	endPos			= 0;
	usingSlider		= false;

	lineSpacing		= 12;
	dblClickTime	= 500;	
	
	selection_has_changed = false;
}


//------------------------------------------------
void guiTypeFileLister::setup(string listerName, simpleFileLister * listerPtr , float listerWidth, float listerHeight){
	lister  =  listerPtr;
	name    =  listerName;
	makeXmlNameFromDisplayName();
	
	updateText();

	setDimensions(listerWidth, listerHeight);
	outlineColor.selected = outlineColor.color;
}

//-----------------------------------------------
void guiTypeFileLister::notify(){
	guiCallbackData cbVal;
	cbVal.setup(xmlName, name);
	cbVal.addValueI(selection);
	cbVal.addValueS(lister->getSelectedPath());
	cbVal.addValueS(lister->getSelectedName());
	ofNotifyEvent(guiEvent,cbVal,this);
}

//-----------------------------------------------.
void guiTypeFileLister::updateGui(float x, float y, bool firstHit, bool isRelative){
	if ( isLocked() )
		return;
	
	if( firstHit && x < hitArea.x + sliderWidth){
		usingSlider = true;
	}

	if( state == SG_STATE_SELECTED){
		if( usingSlider ){
			pct = ( y - ( hitArea.y ) ) / hitArea.height;
			pct = ofClamp(pct, 0, 1);
		}else if( x > hitArea.x + sliderWidth && firstHit){
			int numVisible = endPos - startPos;

			float ypos = y - hitArea.y;
			selectPct  = ( ypos ) / (float)(numVisible * lineSpacing);
			selectPct  = ofClamp(selectPct, 0, 1);

			int select =  startPos + selectPct * (float)numVisible;

			if( select == selectionTmp && (ofGetElapsedTimeMillis() - lastClickTime) < dblClickTime ){
				selection = select;
				lister->setSelectedFile(selection);
				selection_has_changed = true;
				notify();
			}

			selectionTmp  = select;
			lastClickTime = ofGetElapsedTimeMillis();
		}
	}
}

//-----------------------------------------------.
void guiTypeFileLister::release(float x, float y, bool isRelative){
	guiBaseObject::release( x,  y, isRelative);
	usingSlider = false;
}

//-----------------------------------------------.
void guiTypeFileLister::drawRecords(float x, float y, float width, float height){
	if( lister == NULL)return;
	if( lister->entries.size() == 0)return;
	
	float textW = MAX(5, displayText.stringWidth("abcdefg")/7.0);
	float textH = MAX(lineSpacing, displayText.getTextSingleLineHeight() + 2);
	lineSpacing = textH;
	
	ofPushStyle();
		float yPos = textH;
		startPos = (float)(lister->entries.size()-1) * (pct);
		endPos = 0;

		int numCanFit = (height / textH)- 1;
		endPos = startPos + numCanFit;
		endPos = MIN(lister->entries.size(), endPos);

		for(int i = startPos; i < endPos; i++){

			string str = lister->entries[i].filename;
			if( str.length() * textW > width ){
				int newLen = (float)width / textW;

				str = str.substr(0, newLen);
			}

			if( i == selection ){
				ofPushStyle();
					ofFill();
					ofSetColor( fgColor.getSelectedColor() );
					ofDrawRectangle(x, y+yPos+4, width-5, -textH);
				ofPopStyle();
			}else if( i == selectionTmp){
				ofPushStyle();
					ofNoFill();
					ofSetColor(outlineColor.getColor());
					ofDrawRectangle(x, y+yPos+4, width-2, -textH);
				ofPopStyle();
			}

			ofSetColor(textColor.getColor());

			displayText.renderString(str, x, y + yPos);
			yPos += textH;
			//if(yPos+lineSpacing >= height)break;
		}
	ofPopStyle();
}
 
//-----------------------------------------------.
void guiTypeFileLister::render(){
	ofPushStyle();

		ofPushMatrix();
			//draw the background
			ofFill();
			ofSetColor(bgColor.getColor());
			ofDrawRectangle(boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height);

			ofSetColor(textColor.getColor());
			guiBaseObject::renderText();

			ofFill();
			ofSetColor(fgColor.getColor());
			ofDrawRectangle(hitArea.x, hitArea.y + (hitArea.height - 5) * pct, sliderWidth, 5);

			ofNoFill();
			ofSetColor(outlineColor.getColor());
			ofDrawRectangle(boundingBox.x, boundingBox.y, boundingBox.width, boundingBox.height);
			ofDrawRectangle(hitArea.x , hitArea.y, sliderWidth, hitArea.height);

			ofSetColor(textColor.getColor());
			if(lister != NULL)drawRecords(hitArea.x+sliderWidth + 5, hitArea.y, boundingBox.width-(sliderWidth + 5), boundingBox.height);

		ofPopMatrix();

	ofPopStyle();
}

void guiTypeFileLister::clearSelection()
{

	selection = -1;
}