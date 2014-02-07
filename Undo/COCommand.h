/*
	Copyright (C) 2013 Eric Wasylishen, Quentin Mathe

	Date:  September 2013
	License:  MIT  (see COPYING)
 */

#import <Foundation/Foundation.h>
#import <EtoileFoundation/ETUUID.h>
#import <CoreObject/COTrack.h>

@class COEditingContext, COUndoTrack;

extern NSString * const kCOCommandType;
extern NSString * const kCOCommandUUID;
extern NSString * const kCOCommandStoreUUID;
extern NSString * const kCOCommandPersistentRootUUID;
extern NSString * const kCOCommandTimestamp;


/**
 * @group Undo
 * @abstract A command represents a committed change in an editing context
 *
 * For each store change operation (e.g. branch creation, new revision etc.), 
 * there is a distinct command in the COCommand class hierarchy.
 *
 * A commit is not atomic, if it spans several persistent roots. Non-atomic 
 * commits are represented as a COCommandGroup that contains one or more command 
 * objects to describe each store structure change independently.<br />
 * If you make multiple store structure changes on a single persistent root 
 * (e.g. branch creation and new revision at the same time), the command group 
 * is going to contain several commands just for a single persistent root.
 */
@interface COCommand : NSObject <COTrackNode>
{
	COUndoTrack __weak *_parentUndoTrack;
}


/** @taskunit Basic Properties */


/**
 * <override-subclass />
 * A localized string describing the command.
 *
 * For example, -[COCommandDeletePersistentRoot kind] returns <em>Persistent 
 * Root Deletion</em>.
 */
@property (nonatomic, readonly) NSString *kind;
/**
 * The undo track on which the command was recorded.
 *
 * Never returns nil once the command has been recorded, see 
 * -[COUndoTrack recordCommand:].
 */
@property (nonatomic, readwrite, weak) COUndoTrack *parentUndoTrack;


/** @taskunit Applying and Reverting Changes */


/**
 * <override-subclass />
 * Returns a command that represents an inverse action.
 *
 * You can use the inverse to unapply the receiver changes in an editing context.
 *
 * <code>[[command inverse] inverse]</code> must be equal 
 * <code>[command inverse]</code>.
 */
- (COCommand *) inverse;
/**
 * <override-dummy />
 * Returns a new command which can be applied or unapplied with semantics 
 * and observable results identical to the receiver in the given editing 
 * context.
 *
 * By default, returns self, but can be overriden to return a new command.
 *
 * This method exists to support turning a selective undo into a linear undo, 
 * and a selective redo into a linear redo. This results in a simpler command 
 * sequence or history in the undo track.
 *
 * Command Rewriting is unrelated to Undo Coalescing (see 
 * -[COUndoTrack beginUndoCoalescing]).
 *
 * Note: for now, this feature is disabled.
 */
- (COCommand *) rewrittenCommandAfterCommitInContext: (COEditingContext *)aContext;

// FIXME: Perhaps distinguish between edits that can't be applied and edits that
// are already applied. (e.g. "create branch", but that branch already exists)

/** 
 * <override-subclass />
 * Returns whether the receiver changes can be applied to the editing context.
 */
- (BOOL) canApplyToContext: (COEditingContext *)aContext;
/**
 * <override-subclass />
 * Applies the receiver changes to the editing context.
 */
- (void) applyToContext: (COEditingContext *)aContext;


/** @taskunit Framework Private */


/**
 * <override-never />
 * Returns a command deserialized from a property list.
 *
 * See -initWithPropertyList:parentUndoTrack:.
 */
+ (COCommand *) commandWithPropertyList: (id)aPlist parentUndoTrack: (COUndoTrack *)aParent;
/**
 * <init />
 * Initializes and returns a command deserialized from a property list.
 */
- (id) initWithPropertyList: (id)plist parentUndoTrack: (COUndoTrack *)aParent;
/**
 * Returns the receiver serialized as a property list.
 */
- (id) propertyList;

@end


/**
 * @group Undo
 * @abstract A command representing a single store structure change
 *
 * A single command corresponds to an atomic operation inside a commit 
 * (e.g. just a branch creation or just a new revision).
 *
 * For each commit, single commands are grouped into a COCommandGroup.
 */
@interface COSingleCommand : COCommand <NSCopying>
{
    ETUUID *_storeUUID;
    ETUUID *_persistentRootUUID;
}


/** @taskunit Basic Properties */


/**
 * The UUID of the store against which the changes were or would be committed 
 * (for an inverse).
 */
@property (nonatomic, copy) ETUUID *storeUUID;
/**
 * The UUID of the persistent root to which the changes were or would be applied 
 * (for an inverse).
 */
@property (nonatomic, copy) ETUUID *persistentRootUUID;


/** @taskunit Framework Private */


/**
 * Returns a new command equal to the receiver.
 */
- (id) copyWithZone: (NSZone *)zone;

@end

