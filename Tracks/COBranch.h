/*
	Copyright (C) 2011 Christopher Armstrong

	Author:  Christopher Armstrong <carmstrong@fastmail.com.au>,
	         Quentin Mathe <quentin.mathe@gmail.com>
	Date:  September 2011
	License:  Modified BSD  (see COPYING)
 */

#import <Foundation/Foundation.h>
#import <CoreObject/COTrack.h>

@class COObject, CORevision, COPersistentRoot, COBranchInfo, COObjectGraphContext;

/**
 * A persistent history track on an object.
 * 
 * Unlike COHistoryTrack, COCommitTrack is built to:
 * <list>
 * <item>track a single object</item>
 * <item>persist the track nodes and the current node</item>
 * <item>move the current node to the next or previous track node, to move the 
 * undo/redo pointer in the track timeline</item>
 * </list>
 */
@interface COBranch : NSObject
{
	@private
    ETUUID *_UUID;
    
    /**
     * Weak reference
     */
	COPersistentRoot *_persistentRoot;

    BOOL _isCreated;
    
    /** 
     * If _isCreated is NO, this is the parent revision to use for the branch.
     *
     * If this is different than the current revision for this branch
     * recorded in _persistentRoot's _savedState, it means the user has reverted
     * to a past revision.
     */
	CORevisionID *_currentRevisionID;
    
    /**
     * If different than the metadata for this branch in _persistentRoot's _savedState,
     * then a metadata change is staged for commit.     
     */
    NSDictionary *_metadata;
    
    COObjectGraphContext *_objectGraph;
    
    BOOL _deleted;
}

/** @taskunit Branch Kind */

/**
 * Returns whether the branch represents a cheap copy.
 */
@property (readonly, nonatomic) BOOL isCopy;
/**
 * Returns whether the receiver is the current branch of its persistent root.
 */
@property (readonly, nonatomic) BOOL isCurrentBranch;

/**
 * Returns whether the receiver was the first branch of its persistent root
 */
@property (readonly, nonatomic) BOOL isTrunkBranch;

/** @taskunit Basic Properties */

/**
 * The branch UUID.
 *
 * The UUID is never nil.
 */
@property (readonly, nonatomic) ETUUID *UUID;
/**
 * The branch label (used as the branch name in most cases).
 */
@property (readonly, nonatomic) NSString *label;
/**
 * The metadata attached to the branch.
 *
 * Any changes to the metadata is saved on the next object graph context commit.
 */
@property (readwrite, retain, nonatomic) NSDictionary *metadata;
/** 
 * The branch deletion status.
 *
 * If the branch is marked as deleted, the deletion is committed to the store 
 * on the next persistent root commit.
 */
@property (readwrite, nonatomic, getter=isDeleted, setter=setDeleted:) BOOL deleted;

/** @taskunit History */

/**
 * The parent branch from which the receiver is derived.
 *
 * If the parent branch is nil, this means the receiver is a branch that was 
 * created at the same time than its persistent root. The parent revision is 
 * also nil in this case.
 *
 * For a cheap copy, the parent branch is never nil.
 */
@property (readonly, nonatomic) COBranch *parentBranch;
/**
 * The revision at which the receiver was forked from the parent branch.
 *
 * If the parent revision is nil, this means the receiver is a branch that was 
 * created at the same time than its persistent root. The parent branch is also 
 * nil in this case.
 *
 * FIXME: The name "parent" is confusing, it is not the same as the parent of
 * a revision. In COSQLiteStore's terminology this is the "tail" of the branch.
 */
@property (readonly, nonatomic) CORevision *parentRevision;
/**
 * The revision bound to the state loaded in the object graph context.
 *
 * If the branch is uncommitted, the current revision is nil.
 */
@property (readwrite, retain, nonatomic) CORevision *currentRevision;
/**
 * The revision bound to the most recent commit in the branch.
 *
 * In the store terminology, this is the branch head revision.
 */
@property (readonly, nonatomic) CORevision *newestRevision;

/** @taskunit Persistent Root and Object Graph Context */

/**
 * The persistent root owning the branch.
 */
@property (readonly, nonatomic) COPersistentRoot *persistentRoot;
/**
 * The object graph context owned by the branch.
 */
@property (readonly, nonatomic) COObjectGraphContext *objectGraph;

/** @taskunit Undo / Redo */

- (void)undo;
- (void)redo;

/** @taskunit Creating Branches and Cheap copies */

// TODO: Convert these methods to logging the change in the editing context and saving it
// at commit time.

/**
 * Returns a new commit track by branching the receiver last revision and using 
 * the given label.
 *
 * The receiver must be committed.
 *
 * See also -makeBranchWithLabel:atRevision:.
 */
- (COBranch *)makeBranchWithLabel: (NSString *)aLabel;
/**
 * Returns a new commit track by branching a particular revision and using
 * the given label.
 *
 * The revision must belong to the receiver track, otherwise a 
 * NSInvalidArgumentException is raised.
 *
 * The branch creation results in a new revision on the store structure track. 
 * See -[COStore createCommitTrackWithUUID:name:parentRevision:rootObjectUUID:persistentRootUUID:isNewPersistentRoot:].
 *
 * You can assign the returned commit track to the receiver persistent root to 
 * switch the current branch. For example:
 *
 * <example>
 * [persistentRoot setCommitTrack: [[persistentRoot commitTrack] makeBranchWithLabel: @"Sandbox"]];
 * </example>
 *
 * One restriction is that the receiver must be committed - not a newly
 * created branch, or the default branch of a persistent root. This is just for
 * implementation simplicity, not a fundamental design limitation.
 */
- (COBranch *)makeBranchWithLabel: (NSString *)aLabel atRevision: (CORevision *)aRev;
/**
 * Returns a new persistent root bound to a new commit track by branching a 
 * particular revision.
 * 
 * The resulting persistent root is known as a cheap copy, because the copy 
 * doesn't cause the history leading to the new persistent root state to be 
 * duplicated in the store.
 *
 * Although we usually don't call a cheap copy a branch, the new commit track 
 * is a branch from the viewpoint of the history graph.
 *
 * The revision must belong to the receiver track, otherwise a
 * NSInvalidArgumentException is raised.
 *
 * The receiver must be committed.
 */
- (COPersistentRoot *)makeCopyFromRevision: (CORevision *)aRev;

/** @taskunit Merging Between Tracks */

/**
 * This method is not yet implemented.
 */
- (BOOL)mergeChangesFromTrack: (COBranch *)aSourceTrack;
/**
 * This method is not yet implemented.
 */
- (BOOL)mergeChangesFromRevision: (CORevision *)startRev
							  to: (CORevision *)endRev
						 ofTrack: (COBranch *)aSourceTrack;
/**
 * This method is not yet implemented.
 */
- (BOOL)mergeChangesFromRevisionSet: (NSSet *)revs
							ofTrack: (COBranch *)aSourceTrack;

/** @taskunit Private */

- (id)        initWithUUID: (ETUUID *)aUUID
        objectGraphContext: (COObjectGraphContext *)anObjectGraphContext
            persistentRoot: (COPersistentRoot *)aPersistentRoot
parentRevisionForNewBranch: (CORevisionID *)parentRevisionForNewBranch;

- (void)didMakeInitialCommitWithRevisionID: (CORevisionID *)aRevisionID;
- (void) saveCommitWithMetadata: (NSDictionary *)metadata;
- (void)discardAllChanges;
- (void)discardChangesInObject: (COObject *)object;
- (BOOL) isBranchUncommitted;

@end