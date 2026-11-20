.class public final synthetic Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda9;
.super Ljava/lang/Object;
.source "D8$$SyntheticClass"

# interfaces
.implements Landroid/content/DialogInterface$OnClickListener;


# instance fields
.field public final synthetic f$0:Lcom/racas/black/BlackActivity;

.field public final synthetic f$1:Lcom/racas/black/UpdateChecker$Release;


# direct methods
.method public synthetic constructor <init>(Lcom/racas/black/BlackActivity;Lcom/racas/black/UpdateChecker$Release;)V
    .locals 0

    .line 0
    invoke-direct {p0}, Ljava/lang/Object;-><init>()V

    iput-object p1, p0, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda9;->f$0:Lcom/racas/black/BlackActivity;

    iput-object p2, p0, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda9;->f$1:Lcom/racas/black/UpdateChecker$Release;

    return-void
.end method


# virtual methods
.method public final onClick(Landroid/content/DialogInterface;I)V
    .locals 1

    .line 0
    iget-object v0, p0, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda9;->f$0:Lcom/racas/black/BlackActivity;

    iget-object p0, p0, Lcom/racas/black/BlackActivity$$ExternalSyntheticLambda9;->f$1:Lcom/racas/black/UpdateChecker$Release;

    invoke-static {v0, p0, p1, p2}, Lcom/racas/black/BlackActivity;->$r8$lambda$okZseP0z6f850zEVOyx2-WlMS1I(Lcom/racas/black/BlackActivity;Lcom/racas/black/UpdateChecker$Release;Landroid/content/DialogInterface;I)V

    return-void
.end method
