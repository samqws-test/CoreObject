#import "COSynchronizerJSONUtils.h"
#import "COSynchronizerRevision.h"

@implementation COSynchronizerJSONUtils

+ (NSString *) serializePropertyList: (id)plist
{
	NSData *data = [NSJSONSerialization dataWithJSONObject: plist options: 0 error: NULL];
	return [[NSString alloc] initWithData: data encoding: NSUTF8StringEncoding];
}

+ (id) deserializePropertyList: (NSString *)aString
{
	NSData *data = [aString dataUsingEncoding: NSUTF8StringEncoding];
	return [NSJSONSerialization JSONObjectWithData: data options:0 error: NULL];
}

+ (id) propertyListForRevisionsArray: (NSArray *)revs
{
	NSMutableArray *array = [NSMutableArray array];
	for (COSynchronizerRevision *revision in revs)
	{
		id revisionPropertyList = [revision propertyList];
		[array addObject: revisionPropertyList];
	}
	return array;
}

+ (NSArray *) revisionsArrayForPropertyList: (id)aPropertylist
{
	NSMutableArray *array = [NSMutableArray array];
	for (id revisionPropertyList in aPropertylist)
	{
		COSynchronizerRevision *rev = [[COSynchronizerRevision alloc] initWithPropertyList: revisionPropertyList];
		[array addObject: rev];
	}
	return array;
}

@end